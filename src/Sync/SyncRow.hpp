//
// Created by sr-71 on 29/05/2024.
//

#pragma once
#include <any>
#include <LinearProgressBar.hpp>
#include <QStandardItem>
#include <RcloneFile.hpp>

class SyncRow
{
public:
	explicit SyncRow(const std::string &src, const std::string &dst);

	void setTransferData(const ire::json_log::stats::transfer &transfer);

	void finish();

	void error(const std::string &message);

	enum State
	{
		None,
		Syncing,
		Finished,
		Error
	};

	State state() const;

	bool compare(int collumn, Qt::SortOrder order,const SyncRow& other) const;

	QVariant data(int column) const;

private:
	State _state{None};
	uint64_t _start_time{0};
	QString _src;
	char _progress;
	QString _dst;
	uint64_t _size;
	uint64_t _remainingTime;
	uint64_t _ellapsedTime;
	uint64_t _speed;
	uint64_t _speed_avg;

};
