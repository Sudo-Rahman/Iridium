//
// Created by Rahman on 12/04/2023.
//

#include <QStandardItemModel>
#include <QHeaderView>
#include <QMenu>
#include <Global.hpp>
#include <QMessageBox>
#include "TaskTreeView.hpp"

#include "IridiumApp.hpp"
#include "TaskRowChild.hpp"
#include "TaskRowParent.hpp"

using namespace std;
using namespace boost;
using namespace iridium::rclone;
using namespace iridium::rclone::entities;

std::unordered_map<size_t, Tasks> TaskTreeView::_tasks;

void TaskTreeView::cancelTask()
{
	auto indexes = QTreeView::selectedIndexes();
	for (int i = 0; i < indexes.size(); i = i + 10)
	{
		auto id = indexes[i].siblingAtColumn(0);
		auto it = _tasks.find(id.data(Qt::UserRole + 1).toULongLong());
		if (it not_eq _tasks.end())
		{
			if (it->second.rclone->is_running())
				it->second.rclone->stop();
			it->second.parent->cancel();
			for (const auto &child: it->second.children)
				child.second->cancel();
		}
	}
}

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
		{
				tr("Source"), tr("Destination"), tr("Taille"), tr("Type"), tr("Progression"),
				tr("Statut"), tr("Temps restant"),
				tr("Temps écoulé"), tr("Vitesse"), tr("Vitesse moyenne")
		});

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
		// get the selected rows size
		auto size = QTreeView::selectedIndexes().size() / header()->count();
		auto cancel = menu.addAction(
			size > 1 ? QObject::tr("Annuler les %1 tâches").arg(size) : QObject::tr("Annuler la tâche"));
		cancel->setIcon(QIcon(":/resources/cancel.png"));

		auto remove = menu.addAction(
			size > 1 ? QObject::tr("Supprimer les %1 tâches").arg(size) : QObject::tr("Supprimer la tâche"));
		remove->setIcon(QIcon(":/resources/trash.png"));
		connect(remove, &QAction::triggered, this, [this]()
		{
			auto indexes = QTreeView::selectedIndexes();
			for (long long i = indexes.size() / 10; i > 0; i--)
			{
				auto id = indexes[i].siblingAtColumn(0);
				auto it = _tasks.find(id.data(Qt::UserRole + 1).toULongLong());
				if (it not_eq _tasks.end())
				{
					if (it->second.rclone->is_running())
						it->second.rclone->stop();
					_model->removeRow(indexes[i].row(), indexes[i].parent());
					_tasks.erase(it);
				}
			}
		});

		auto action = menu.exec(QCursor::pos());

		if (action == cancel)
		{
			QMessageBox info = QMessageBox(QMessageBox::Information, tr("Annuler la tâche"),
			                               tr("Voulez-vous vraiment annuler la tâche ?"), {}, this);
			info.addButton(tr("Oui"), QMessageBox::YesRole);
			info.addButton(tr("Non"), QMessageBox::NoRole);
			if (info.exec() == QMessageBox::NRoles)
				return;

			cancelTask();
		}
	});
}

/**
 * @brief add a task to the view
 * @param src
 * @param dst
 * @param rclone
 */
void TaskTreeView::addTask(const RcloneFile &src, const RcloneFile &dst, const process_ptr &rclone,
                           TaskRowParent::taskType type)
{
	auto idParent = boost::hash<std::string>{}(src.absolute_path() + dst.absolute_path() + to_string(type));

	// if the task already exist
	if (_tasks.find(idParent) not_eq _tasks.end())
	{
		// kill the task and remove it from the view
		if (_tasks[idParent].rclone->is_running())
			_tasks[idParent].rclone->stop();
		_tasks[idParent].parent->cancel();
		_model->removeRow(_tasks[idParent].parent->first()->row(),
		                  _model->indexFromItem(_tasks[idParent].parent->first()->parent()));
		_tasks.erase(idParent);
	}

	// create the task
	auto task = std::make_unique<TaskRowParent>(src, dst, ire::json_log(), type);
	_model->appendRow(*task);
	task->first()->setData((qulonglong)idParent, Qt::UserRole + 1);
	setIndexWidget(task->progressBarIndex(), task->progressBar());
	_tasks.insert({idParent, Tasks{std::move(task), rclone, {}}});

	auto parser = irp::json_log_parser::create(
		new irp::json_log_parser([this, src, dst, type, idParent](const ire::json_log &log)
		{
			IridiumApp::runOnMainThread([=,log = std::move(log)]
			{
				if (log.level() == ire::json_log::log_level::error)
				{
					// chek if task is in the map
					if (_tasks.find(idParent) == _tasks.end())
						return;
					size_t errId;
					if (src.isDir())
						errId = boost::hash<std::string>{}(
							src.absolute_path() + log.object() + dst.absolute_path() + log.object());
					else
						errId = boost::hash<std::string>{}(src.absolute_path() + dst.absolute_path() + to_string(type));
					// if the child is the parent
					if (idParent == errId)
					{
						_tasks[idParent].parent->error(log.message());
						_tasks[idParent].parent->setMessageToolTip(log.message());
						if (not _tasks[idParent].children.empty())
							for (auto &child: _tasks[idParent].children)
								child.second->error("");
						return;
					}
					auto it = _tasks[idParent].children.find(errId);
					if (it != _tasks[idParent].children.end())
					{
						it->second->error(log.message());
						it->second->setMessageToolTip(log.message());
						return;
					}
					auto task = std::make_unique<TaskRowChild>(src, dst, ire::json_log::stats::transfer());
					task->error(log.message());

					setIndexWidget(task->progressBarIndex(), task->progressBar());
					_tasks[idParent].parent->first()->appendRow(*task);
					_tasks[idParent].children.insert({errId, std::move(task)});
					return;
				}

				if (log.get_stats() == nullptr)
					return;

				auto it = _tasks.find(idParent);
				if (it != _tasks.end())
					it->second.parent->updateData(log);

				if (log.get_stats()->transferring.empty() or not src.isDir())
					return;

				size_t childId;
				std::vector<size_t> hashList;
				for (const auto &transfer: log.get_stats()->transferring)
				{
					// hash the name of the file
					childId = boost::hash<std::string>{}(
						src.absolute_path() + transfer.name + dst.absolute_path() + transfer.name);
					hashList.emplace_back(childId);

					// if parent task not exist, return
					if (_tasks.find(idParent) == _tasks.end())
						return;

					auto it = _tasks[idParent].children.find(childId);

					if (std::find(hashList.begin(), hashList.end(), childId) != hashList.end())
					{
						// if the child task already exist
						if (it != _tasks[idParent].children.end())
							it->second->updateData({transfer});
						// if the child task not exist in the task list, create it
						else
						{
							auto childSrc = RcloneFile(src.parent(), transfer.name.c_str(), transfer.size, false,
							                           QDateTime(), src.getRemoteInfo());
							auto childDst = RcloneFile(dst.parent(), transfer.name.c_str(), transfer.size, false,
							                           QDateTime(), dst.getRemoteInfo());
							auto taskChild = std::make_unique<TaskRowChild>(
								std::move(childSrc), std::move(childDst), transfer);
							_tasks[idParent].parent->first()->appendRow(*taskChild);
							setIndexWidget(taskChild->progressBarIndex(), taskChild->progressBar());
							_tasks[idParent].children.insert({childId, std::move(taskChild)});
						}
					}
				}
				// if the child task already in the list but not in the transferring array, terminate it
				for (auto it = _tasks[idParent].children.begin(); it != _tasks[idParent].children.end(); ++it)
				{
					if (std::find(hashList.begin(), hashList.end(), it->first) == hashList.end())
						if (it->second->state() != TaskRowChild::Error)
							IridiumApp::runOnMainThread([it] { it->second->finished(); });
				}
			});
		}));

	rclone->every_line_parser(std::move(parser));
	rclone->on_finish([src, dst, rclone, type, this](int exit)
	{
		size_t id;
		id = boost::hash<std::string>{}(
			src.absolute_path() + dst.absolute_path() + to_string(type));

		auto it = _tasks.find(id);
		if (it == _tasks.end())
			return;
		IridiumApp::runOnMainThread([=]
		{
			if (exit == 0)
			{
				it->second.parent->finished();
				for (auto &child: it->second.children)
					child.second->finished();
			}
			else
			{
				it->second.parent->error("");
				it->second.parent->setMessageToolTip(rclone->get_error().empty() ? "" : rclone->get_error().back());
				it->second.parent->progressBar()->error();
			}
			if (it->second.parent->state() not_eq TaskRowChild::State::Error)
				it->second.parent->terminate();
		});
	});
	rclone->add_option(option::logging::use_json_log(), option::logging::verbose());
	rclone->execute();
}
