//
// Created by sr-71 on 12/04/2023.
//

#include <QLabel>
#include <QStandardItemModel>
#include <QHeaderView>
#include "TaskTreeView.hpp"
#include "TaskRow.hpp"
#include <boost/functional/hash.hpp>

/**
 * @brief TaskTreeView::TaskTreeView
 * @param parent
 */
TaskTreeView::TaskTreeView(QWidget *parent) : QTreeView(parent)
{
	setAlternatingRowColors(true);
	setUniformRowHeights(true);

	m_model = new QStandardItemModel(0, 9, this);
	QTreeView::setModel(m_model);

	setHeaderHidden(false);
	setRootIsDecorated(false);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_model->setHorizontalHeaderLabels(
		{tr("Source"), tr("Destination"), tr("Taille"), tr("Progression"),
		 tr("Statut"), tr("Temps restant"),
		 tr("Temps écoulé"), tr("Vitesse"), tr("Vitesse moyenne")});

	// set the size of the columns
	header()->setMinimumSectionSize(100);
}

/**
 * @brief add a task to the view
 * @param src
 * @param dst
 * @param rclone
 */
void TaskTreeView::addTask(const QString &src, const QString &dst, const RclonePtr &rclone)
{
	connect(rclone.get(), &Rclone::taskProgress, this, [this, src, dst, rclone](const boost::json::object &obj)
	{
		boost::json::array transfer;
		try
		{ transfer = obj.at("stats").at("transferring").as_array(); }
		// if invalid argument
		catch (const boost::wrapexcept<std::invalid_argument> &e)
		{
			std::cout << e.what() << std::endl;
			return;
		}
		// if out of range
		catch (const boost::wrapexcept<std::out_of_range> &e)
		{
			std::cout << "no Data" << std::endl;
		}

		std::vector<size_t> hashList;
		for (const auto &item: transfer)
		{
			// hash the name of the file
			auto id = boost::hash<std::string>{}(item.at("name").as_string().c_str());
			hashList.emplace_back(id);

			// if the task already exist
			if (m_tasks.find(id) != m_tasks.end())
				m_tasks[id]->updateData(item.as_object());
			// if not exist, create a new one
			else
			{
				auto task = std::make_shared<TaskRow>(src, dst, item.as_object(), id);
				m_tasks[id] = task;
				m_model->appendRow(*task);
				setIndexWidget(task->progressBarIndex(), task->progressBar());
			}
		}
		for (const auto &item: m_tasks)
		{
			// if the task is not in the list, continue
			if (!item.second->src().contains(src) or !item.second->dest().contains(dst))
				continue;
			// terminate the task that are not in the list
			if (std::find(hashList.begin(), hashList.end(), item.first) == hashList.end())
				item.second->terminate();
		}
	});
}
