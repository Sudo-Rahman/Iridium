//
// Created by sr-71 on 29/05/2024.
//

#include "SyncRow.hpp"

#include "Utility/Utility.hpp"

using namespace std;

SyncRow::SyncRow(const std::string &src, const std::string &dst, uint32_t row) : _row(row)
{
	_size = 0;
	_progress = 0;
	_state = None;
	_speed = 0;
	_speed_avg = 0;
	_remainingTime = 0;
	_ellapsedTime = 0;
	_src = QString::fromStdString(src);
	_dst = QString::fromStdString(dst);
}

using namespace std::chrono;

void SyncRow::setTransferData(const ire::json_log::stats::transfer &transfer)
{
	if (_state == None) { _start_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count(); }
	_state = Syncing;
	_progress = transfer.percentage;

	_size = transfer.size;

	_remainingTime = transfer.speed == 0
		                 ? uint64_t(0)
		                 : static_cast<uint64_t>((transfer.size - transfer.bytes) / transfer.speed);

	auto now = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
	_ellapsedTime = now - _start_time;

	_speed = transfer.speed;
	_speed_avg = transfer.speed_avg;
}

void SyncRow::finish()
{
	_progress = 100;
	_state = Finished;
}

void SyncRow::error(const std::string &message)
{
	_state = Error;
	_progress = -1;
}

uint32_t SyncRow::row() const { return _row; }

SyncRow::State SyncRow::state() const { return _state; }

bool SyncRow::compare(int collumn, Qt::SortOrder order, const SyncRow &other) const
{
	switch (collumn)
	{
		case 0:
			if (order == Qt::AscendingOrder)
				return _src < other._src;
			return _src > other._src;
		case 1:
			if (order == Qt::AscendingOrder)
				return _progress < other._progress;
			return _progress > other._progress;
		case 2:
			if (order == Qt::AscendingOrder)
				return _dst < other._dst;
			return _dst > other._dst;
		case 3:
			if (order == Qt::AscendingOrder)
				return _size < other._size;
			return _size > other._size;
		case 4:
			if (order == Qt::AscendingOrder)
				return _remainingTime < other._remainingTime;
			return _remainingTime > other._remainingTime;
		case 5:
			if (order == Qt::AscendingOrder)
				return _ellapsedTime < other._ellapsedTime;
			return _ellapsedTime > other._ellapsedTime;
		case 6:
			if (order == Qt::AscendingOrder)
				return _speed < other._speed;
			return _speed > other._speed;
		case 7:
			if (order == Qt::AscendingOrder)
				return _speed_avg < other._speed_avg;
			return _speed_avg > other._speed_avg;
		default:
			return false;
	}
}

QVariant SyncRow::data(int column) const
{
	switch (column)
	{
		case 0:
			return _src;
		case 1:
			return _progress;
		case 2:
			return _dst;
		case 3:
			return Iridium::Utility::sizeToString(_size).c_str();
		case 4:
			{
				// remainingTime to hh:mm:ss
				auto seconds = _remainingTime % 60;
				auto minutes = _remainingTime / 60 % 60;
				auto hours = _remainingTime / 3600 % 24;

				return QString("%1:%2:%3")
						.arg(hours, 2, 10, QChar('0'))
						.arg(minutes, 2, 10, QChar('0'))
						.arg(seconds, 2, 10, QChar('0'));
			}
		case 5:
			{
				// ellapsedTime to hh:mm:ss
				auto seconds = _ellapsedTime % 60;
				auto minutes = _ellapsedTime / 60 % 60;
				auto hours = _ellapsedTime / 3600 % 24;
				return QString("%1:%2:%3")
						.arg(hours, 2, 10, QChar('0'))
						.arg(minutes, 2, 10, QChar('0'))
						.arg(seconds, 2, 10, QChar('0'));
			}
		case 6:
			return (Iridium::Utility::sizeToString(_speed) + "/s").c_str();
		case 7:
			return (Iridium::Utility::sizeToString(_speed_avg) + "/s").c_str();
		default: return QVariant();
	}
}
