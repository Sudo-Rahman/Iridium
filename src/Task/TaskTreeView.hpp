//
// Created by sr-71 on 12/04/2023.
//

#ifndef IRIDIUM_TASKTREEVIEW_HPP
#define IRIDIUM_TASKTREEVIEW_HPP

#include <QList>
#include <QTreeView>
#include <QLayout>

class TaskTreeView : public QTreeView
{
Q_OBJECT

	QList<QWidget *> m_tasks;


public:
	explicit TaskTreeView(QWidget *parent = nullptr);

//	void addTask(QWidget *task);
//
//	void removeTask(QWidget *task);
//
//	void clearAllTasks();
//
//	void setTasks(QList<QWidget *> tasks);
//
//	QList<QWidget *> getTasks() const;

};


#endif //IRIDIUM_TASKTREEVIEW_HPP
