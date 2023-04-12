//
// Created by sr-71 on 12/04/2023.
//

#ifndef IRIDIUM_TASKROW_HPP
#define IRIDIUM_TASKROW_HPP

#include <QStandardItem>
#include <QList>
#include <QProgressBar>
#include <QTimer>
#include <boost/json.hpp>
#include <Rclone.hpp>

class TaskRow : public QList<QStandardItem *>
{
	QProgressBar *m_progressBar{};

	QTimer m_elapsedTime{};
	uint64_t m_elapsedTimeCount{};

	QString m_src{};
	QString m_dest{};

	uint64_t m_size{};

	size_t m_id{};

public:

	explicit TaskRow(const QString &src, const QString &dest, const boost::json::object & ,const size_t &id);

	[[nodiscard]] QProgressBar *progressBar()
	{ return m_progressBar; }

	[[nodiscard]] QModelIndex progressBarIndex() const
	{ return this->at(3)->index(); };

	void updateData(const boost::json::object &);

	[[nodiscard]] const size_t &id() const
	{ return m_id; }

	[[nodiscard]] const QString &src() const
	{ return m_src; }

	[[nodiscard]] const QString &dest() const
	{ return m_dest; }

	[[nodiscard]] const uint64_t &size() const
	{ return m_size; }

	void terminate();
};

typedef std::shared_ptr<TaskRow> TaskRowPtr;


#endif //IRIDIUM_TASKROW_HPP
