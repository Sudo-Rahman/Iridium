//
// Created by sr-71 on 31/03/2024.
//

#pragma once


#include <iridium/rclone.hpp>
#include <LinearProgressBar.hpp>
#include <boost/variant2/variant.hpp>
#include <QTimer>
#include <QStandardItem>

class TaskRow : public QList<QStandardItem *>
{
public:
	enum State
	{
		Normal,
		Finished,
		Error,
		Cancelled
	};

	virtual ~TaskRow() = default;

	[[nodiscard]] LinearProgressBar* progressBar() const;

	[[nodiscard]] QModelIndex progressBarIndex() const;

	virtual void updateData(const boost::variant2::variant<ire::json_log, ire::json_log::stats::transfer>&) = 0;

	[[nodiscard]] State state() const;

	void setMessageToolTip(const std::string& message) const;

	virtual void finished() = 0;

	virtual void error(const std::string &message);

	virtual void cancel();

	virtual void terminate();

	virtual void connectTimer();

	virtual  void init();

protected:
	LinearProgressBar * _progressBar{};

	QTimer _elapsed_time{};
	uint64_t _elapsed_time_count{};
	State _state{Normal};

};
