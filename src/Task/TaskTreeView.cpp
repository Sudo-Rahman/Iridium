//
// Created by Rahman on 12/04/2023.
//

#include <QStandardItemModel>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include "TaskTreeView.hpp"
#include "TaskRow.hpp"
#include <boost/chrono.hpp>

using namespace std;
using namespace boost;

/**
 * @brief TaskTreeView::TaskTreeView
 * @param parent
 */
TaskTreeView::TaskTreeView(QWidget *parent) : QTreeView(parent)
{
    setAlternatingRowColors(true);
    setUniformRowHeights(true);


    _model = new QStandardItemModel(0, 10, this);
    QTreeView::setModel(_model);

    setIndentation(30);
    setHeaderHidden(false);
    setRootIsDecorated(true);
    setSortingEnabled(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    _model->setHorizontalHeaderLabels(
            {tr("Source"), tr("Destination"), tr("Taille"), tr("Type"), tr("Progression"),
             tr("Statut"), tr("Temps restant"),
             tr("Temps écoulé"), tr("Vitesse"), tr("Vitesse moyenne")});

    // set the size of the columns
    header()->setMinimumSectionSize(100);

    // set Menu context
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &TaskTreeView::customContextMenuRequested, this, [=, this]
    {
        if (QTreeView::selectedIndexes().empty())
            return;
        for (const auto &index: QTreeView::selectedIndexes())
        {
            if (index.parent().isValid())
                return;
        }
        QMenu menu;
        auto cancel = menu.addAction(QObject::tr("Annuler la tâche"));
        cancel->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));

        auto remove = menu.addAction(QObject::tr("Supprimer la tâche"));
        remove->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        connect(remove, &QAction::triggered, this, [this]()
        {
            auto indexes = QTreeView::selectedIndexes();
            for (long long i = indexes.size() / 10; i > 0; i--)
                _model->removeRows(indexes[i].row(), 1, indexes[i].parent());
        });

        auto action = menu.exec(QCursor::pos());

        if (action == cancel)
        {

            QMessageBox info = QMessageBox(QMessageBox::Information, tr("Annuler la tâche"),
                                           tr("Voulez-vous vraiment annuler la tâche ?"));
            info.addButton(tr("Oui"), QMessageBox::YesRole);
            info.addButton(tr("Non"), QMessageBox::NoRole);
            if (info.exec() == QMessageBox::NRoles)
            {
                return;
            }

            auto indexes = QTreeView::selectedIndexes();
            for (int i = 0; i < indexes.size(); i = i + 10)
            {
                auto id = indexes[i].siblingAtColumn(0);
                auto it = _tasks.find(id.data(Qt::UserRole + 1).toULongLong());
                if (it not_eq _tasks.end())
                {
                    it->second.rclone->kill();
                    it->second.parent->setState(TaskRow::State::Cancelled);
                    for (const auto &child: it->second.children)
                        child.second->setState(TaskRow::State::Cancelled);
                    erase_if(_tasks, [it](const auto &t) { return t.first == it->first; });
                }
            }
            return;
        }
    });

    connect(this, &TaskTreeView::taskFinished, this, [this](std::pair<size_t, Tasks> task)
    {
        if (task.second.parent->state() not_eq TaskRow::State::Error)
            task.second.parent->terminate();
        erase_if(_tasks, [task](const auto &t) { return t.first == task.first; });
    });
}

/**
 * @brief add a task to the view
 * @param src
 * @param dst
 * @param rclone
 */
void TaskTreeView::addTask(const QString &src, const QString &dst, const RclonePtr &rclone,
                           const std::function<void()> &callable, const Rclone::TaskType &type)
{
    auto idParent = boost::hash<std::string>{}(src.toStdString() + dst.toStdString());
    if (_tasks.find(idParent) != _tasks.end())
        return;
    auto task = std::make_shared<TaskRow>(src, dst, boost::json::object(), type, TaskRow::Normal, TaskRow::Parent);
    _tasks.insert({idParent, Tasks{task, rclone, {}}});
    _model->appendRow(*task);
    task->first()->setData((qulonglong) idParent, Qt::UserRole + 1);
    setIndexWidget(task->progressBarIndex(), task->progressBar());


    connect(rclone.get(), &Rclone::taskProgress, this,
            [this, src, dst, rclone, type, idParent](const boost::json::object &obj)
            {
                boost::json::array transfer;
                try
                {
                    if (obj.at("level").as_string() == "error")
                    {
//				cout << obj << endl;
                        if (_tasks.find(idParent) == _tasks.end())
                            return;
                        auto object = obj.at("object").as_string();
                        size_t errId;
                        if (src.back() == '/')

                            errId = boost::hash<std::string>{}(
                                    src.toStdString() + object.c_str() + dst.toStdString() + object.c_str());
                        else
                            errId = boost::hash<std::string>{}(src.toStdString() + dst.toStdString());
                        if (idParent == errId)
                        {
                            _tasks[idParent].parent->setState(TaskRow::Error);
                            _tasks[idParent].parent->setMessageToolTip(obj.at("msg").as_string().c_str());
                            if (not _tasks[idParent].children.empty())
                                for (auto &child: _tasks[idParent].children)
                                    child.second->setState(TaskRow::Error);
                            return;
                        }
                        auto it = _tasks[idParent].children.find(errId);
                        if (it != _tasks[idParent].children.end())
                        {
                            it->second->setState(TaskRow::Error);
                            it->second->setMessageToolTip(obj.at("msg").as_string().c_str());
                            return;
                        }
                        auto task = std::make_shared<TaskRow>(src + object.c_str(), dst + object.c_str(), obj, type,
                                                              TaskRow::Error, TaskRow::Child);
                        _tasks[idParent].children.insert({errId, task});
                        _tasks[idParent].parent->first()->appendRow(*task);
                        setIndexWidget(task->progressBarIndex(), task->progressBar());
                        return;
                    }
                    if (not obj.contains("stats"))
                        return;

                    auto it = _tasks.find(idParent);
                    if (it != _tasks.end())
                        it->second.parent->updateData(obj.at("stats").as_object());

                    transfer = obj.at("stats").at("transferring").as_array();
                }
                    // if invalid argument
                catch (const boost::wrapexcept<std::invalid_argument> &e)
                {
                    std::cout << e.what() << std::endl;
                    return;
                }
                    // if out of range
                catch (const boost::wrapexcept<std::out_of_range> &e) { std::cout << "no Data" << std::endl; }

                size_t childId;
                std::vector<size_t> hashList;
                for (const auto &item: transfer)
                {
                    if (not item.is_object())
                        continue;

                    // hash the name of the file
                    auto name = item.at("name").as_string();
                    childId = boost::hash<std::string>{}(
                            src.toStdString() + name.c_str() + dst.toStdString() + name.c_str());
                    hashList.emplace_back(childId);

                    // if parent task not exist, return
                    if (_tasks.find(idParent) == _tasks.end())
                        return;

                    auto it = _tasks[idParent].children.find(childId);

                    if (std::find(hashList.begin(), hashList.end(), childId) != hashList.end())
                    {
                        // if the child task already exist and is json data
                        if (it != _tasks[idParent].children.end())
                            it->second->updateData(item.as_object());
                            // if the child task not exist in the task list, create it
                        else
                        {
//					cout << obj << endl;
                            TaskRowPtr task;
                            if (src.back() == '/')
                                task = std::make_shared<TaskRow>(src + name.c_str(), dst + name.c_str(),
                                                                 item.as_object(),
                                                                 type, TaskRow::Normal, TaskRow::Child);
                            else
                                task = std::make_shared<TaskRow>(src, dst, item.as_object(),
                                                                 type, TaskRow::Normal, TaskRow::Child);
                            _tasks[idParent].children.insert({childId, task});
                            _tasks[idParent].parent->first()->appendRow(*task);
                            setIndexWidget(task->progressBarIndex(), task->progressBar());
                        }
                    }
                }
                // if the child task already in the list but not in the json data, terminate it
                for (auto it = _tasks[idParent].children.begin(); it != _tasks[idParent].children.end(); ++it)
                {
                    if (std::find(hashList.begin(), hashList.end(), it->first) == hashList.end())
                        if (it->second->state() != TaskRow::Error)
                            it->second->setState(TaskRow::Finished);
                }
            });

    connect(rclone.get(), &Rclone::finished, this,
            [src, dst, rclone, this](int exit)
            {
                boost::json::object json;
                size_t id;
                try
                {
                    id = boost::hash<std::string>{}(
                            src.toStdString() + dst.toStdString());
                } catch (const boost::wrapexcept<std::invalid_argument> &e) { return; }
                auto it = _tasks.find(id);
                if (exit == 0)
                    (*it).second.parent->setState(TaskRow::Finished);
                else
                {
                    (*it).second.parent->setState(TaskRow::Error);
                    (*it).second.parent->setMessageToolTip(rclone->readAllError().back());
                    (*it).second.parent->progressBar()->error();
                }
                emit taskFinished(*_tasks.find(id));
            });
    callable();
}
