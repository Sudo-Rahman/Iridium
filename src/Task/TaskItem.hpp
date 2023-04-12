//
// Created by sr-71 on 12/04/2023.
//

#ifndef IRIDIUM_TASKITEM_HPP
#define IRIDIUM_TASKITEM_HPP

#include <QStandardItem>
#include <QList>
#include <QProgressBar>
#include <QTimer>
#include <boost/json.hpp>
#include <Rclone.hpp>

class TaskItem : public QList<QStandardItem>
{
	QTimer m_timer{};
	QProgressBar *m_progressBar{};
	RclonePtr m_rclone{};
	boost::json::object m_data{};
	QTime m_elapsedTime{};

public:

	TaskItem(RclonePtr rclone);
};


#endif //IRIDIUM_TASKITEM_HPP
