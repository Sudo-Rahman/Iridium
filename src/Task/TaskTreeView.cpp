//
// Created by sr-71 on 12/04/2023.
//

#include <QLabel>
#include <QStandardItemModel>
#include <QHeaderView>
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
	setRootIsDecorated(false);
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

	connect(this, &TaskTreeView::taskFinished, this, [this](std::pair<size_t, Tasks> task)
	{
		task.second.parent->terminate();
		m_tasks.erase(task.first);
	});

	m_thread = boost::thread(
		[this]()
		{
			while (m_isRunning)
			{
				boost::this_thread::sleep_for(boost::chrono::seconds(1));
				for (auto &task: m_tasks)
				{
					auto size = task.second.children.size();
					uint16_t counter = 0;
					for (const auto &child: task.second.children)
					{
						if (child.second->state() == TaskRow::Finished)
							counter++;
					}
					if (counter == size)
						emit taskFinished(task);
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
	connect(rclone.get(), &Rclone::taskProgress, this, [this, src, dst, rclone, type](const boost::json::object &obj)
	{
		boost::json::array transfer;
		size_t idParent = 0;
		try
		{
			if (obj.at("level").as_string() != "info")
				return;
			if (not obj.contains("stats"))
				return;
			idParent = boost::hash<std::string>{}(
				src.toStdString() + dst.toStdString());

			auto it = m_tasks.find(idParent);
			if (it != m_tasks.end())
				it->second.parent->updateData(obj.at("stats").as_object());
			else
			{
				auto task = std::make_shared<TaskRow>(src, dst, obj, idParent, type, TaskRow::Normal, TaskRow::Parent);
				m_tasks.insert({idParent, Tasks{task, {}}});
				m_model->appendRow(*task);
				setIndexWidget(task->progressBarIndex(), task->progressBar());
			}

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
				name.c_str() + std::to_string(item.at("size").as_int64()));
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
					auto task = std::make_shared<TaskRow>(src + name.c_str(), dst + name.c_str(), item.as_object(),
														  childId, type, TaskRow::Normal,
														  TaskRow::Child);
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
				it->second->terminate();
		}
	});

	connect(rclone.get(), &Rclone::taskFinished, this,
			[src, dst, type, this](int exit, const boost::json::object &obj)
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
				if (it == m_tasks.end())
				{
					TaskRowPtr task;
					if (exit == 0)
						task = std::make_shared<TaskRow>(src, dst, obj, id, type, TaskRow::Finished);
					else
						task = std::make_shared<TaskRow>(src, dst, obj, id, type, TaskRow::Error);

					m_tasks.insert({id, Tasks{task, {}}});
					m_model->appendRow(*task);
					setIndexWidget(task->progressBarIndex(), task->progressBar());
					m_tasks.erase(id);
				} else
				{
					it->second.parent->terminate();
					m_tasks.erase(id);
				}
			});
	callable();
}
