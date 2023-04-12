//
// Created by sr-71 on 12/04/2023.
//

#ifndef IRIDIUM_TASKTREEVIEW_HPP
#define IRIDIUM_TASKTREEVIEW_HPP

#include <QList>
#include <boost/unordered_map.hpp>
#include <QTreeView>
#include <QLayout>
#include "TaskRow.hpp"

class TaskTreeView : public QTreeView
{
Q_OBJECT

	boost::unordered_map<size_t, TaskRowPtr> m_tasks{};

	QStandardItemModel *m_model{};


public:
	explicit TaskTreeView(QWidget *parent = nullptr);

	void addTask(const QString &src, const QString &dst, const RclonePtr &rclone);

};


#endif //IRIDIUM_TASKTREEVIEW_HPP
