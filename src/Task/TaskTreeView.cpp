//
// Created by sr-71 on 12/04/2023.
//

#include <QStandardItemModel>
#include <QHeaderView>
#include <QMenu>
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


	m_model = new QStandardItemModel(0, 10, this);
	QTreeView::setModel(m_model);

	setIndentation(30);
	setHeaderHidden(false);
	setRootIsDecorated(true);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_model->setHorizontalHeaderLabels(
		{tr("Source"), tr("Destination"), tr("Taille"), tr("Type"), tr("Progression"),
		 tr("Statut"), tr("Temps restant"),
		 tr("Temps écoulé"), tr("Vitesse"), tr("Vitesse moyenne")});

	// set the size of the columns
	header()->setMinimumSectionSize(100);

	// set Menu context
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &TaskTreeView::customContextMenuRequested, this, [this]
	{
		if (QTreeView::selectedIndexes().empty())
			return;
		QMenu menu;
		menu.addAction(QObject::tr("Supprimer"));
		connect(&menu, &QMenu::triggered, this, [this](QAction *action)
		{
		});
		menu.exec(QCursor::pos());
	});

	connect(this, &TaskTreeView::taskFinished, this, [this](std::pair<size_t, Tasks> task)
	{
		if (task.second.parent->state() not_eq TaskRow::State::Error)
			task.second.parent->terminate();
		erase_if(m_tasks, [task](const auto &t)
		{ return t.first == task.first; });
	});

	m_thread = boost::thread(
		[this]()
		{
			while (true)
			{
				boost::this_thread::sleep_for(boost::chrono::seconds(1));
				boost::this_thread::interruption_point();
				if (m_tasks.empty())
					continue;
				auto it = m_tasks.begin();
				for (; it != m_tasks.end(); ++it)
				{
					if (it->second.isFinished)
						emit taskFinished(*it);
				}
			}
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
	if (m_tasks.find(idParent) != m_tasks.end())
		return;
	auto task = std::make_shared<TaskRow>(src, dst, boost::json::object(), type, TaskRow::Normal, TaskRow::Parent);
	m_tasks.insert({idParent, Tasks{task, {}}});
	m_model->appendRow(*task);
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
						if (m_tasks.find(idParent) == m_tasks.end())
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
							m_tasks[idParent].parent->setState(TaskRow::Error);
							m_tasks[idParent].parent->setMessageToolTip(obj.at("msg").as_string().c_str());
							RcloneManager::release(rclone);
							return;
						}
						auto it = m_tasks[idParent].children.find(errId);
						if (it != m_tasks[idParent].children.end())
						{
							it->second->setState(TaskRow::Error);
							it->second->setMessageToolTip(obj.at("msg").as_string().c_str());
							return;
						}
						auto task = std::make_shared<TaskRow>(src + object.c_str(), dst + object.c_str(), obj, type,
															  TaskRow::Error, TaskRow::Child);
						m_tasks[idParent].children.insert({errId, task});
						m_tasks[idParent].parent->first()->appendRow(*task);
						setIndexWidget(task->progressBarIndex(), task->progressBar());
						return;
					}
					if (not obj.contains("stats"))
						return;

					auto it = m_tasks.find(idParent);
					if (it != m_tasks.end())
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
				catch (const boost::wrapexcept<std::out_of_range> &e)
				{ std::cout << "no Data" << std::endl; }

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
					if (m_tasks.find(idParent) == m_tasks.end())
						return;

					auto it = m_tasks[idParent].children.find(childId);

					if (std::find(hashList.begin(), hashList.end(), childId) != hashList.end())
					{
						// if the child task already exist and is json data
						if (it != m_tasks[idParent].children.end())
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
							m_tasks[idParent].children.insert({childId, task});
							m_tasks[idParent].parent->first()->appendRow(*task);
							setIndexWidget(task->progressBarIndex(), task->progressBar());
						}
					}
				}
				// if the child task already in the list but not in the json data, terminate it
				for (auto it = m_tasks[idParent].children.begin(); it != m_tasks[idParent].children.end(); ++it)
				{
					if (std::find(hashList.begin(), hashList.end(), it->first) == hashList.end())
						if (it->second->state() != TaskRow::Error)
							it->second->setState(TaskRow::Finished);
				}
			});

	connect(rclone.get(), &Rclone::taskFinished, this,
			[src, dst, type, rclone, this](int exit, const boost::json::object &obj)
			{
				boost::json::object json;
				size_t id;
				try
				{
					id = boost::hash<std::string>{}(
						src.toStdString() + dst.toStdString());
				} catch (const boost::wrapexcept<std::invalid_argument> &e)
				{ return; }
				auto it = m_tasks.find(id);
				TaskRowPtr task;
				if (it == m_tasks.end())
				{
					if (exit == 0)
					{
						task = std::make_shared<TaskRow>(src, dst, obj, type, TaskRow::Finished);
						m_model->appendRow(*task);
						setIndexWidget(task->progressBarIndex(), task->progressBar());
					} else
					{
						cout << obj << endl;
						task = std::make_shared<TaskRow>(src, dst, obj, type, TaskRow::Error);
						task->setMessageToolTip(rclone->readAllError().back());
						m_model->appendRow(*task);
						setIndexWidget(task->progressBarIndex(), task->progressBar());
					}
					m_tasks.insert({id, Tasks{task, {}}});
				} else
					m_tasks[id].isFinished = true;
			});
	callable();
}
