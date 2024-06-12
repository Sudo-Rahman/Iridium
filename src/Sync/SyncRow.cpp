//
// Created by sr-71 on 29/05/2024.
//

#include "SyncRow.hpp"

#include "Utility/Utility.hpp"

using namespace std;

SyncRow::SyncRow(const std::string &src, const std::string &dst, uint32_t row) : _row(row)
{
	push_back({nullptr, src.c_str()});
	push_back({int(0), 0});
	push_back({nullptr, dst.c_str()});
	push_back({uint64_t(0), "--"});
	push_back({uint64_t(0), "--"});
	push_back({uint64_t(0), "--"});
	push_back({double_t(0), "--"});
	push_back({double_t(0), "--"});
}

using namespace std::chrono;

void SyncRow::setTransferData(const ire::json_log::stats::transfer &transfer)
{
	if (_state == None) { _start_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count(); }
	_state = Syncing;
	at(1).first = static_cast<int>(transfer.percentage);
	at(1).second = QString::number(transfer.percentage);

	at(3).first = transfer.size;
	at(3).second = Iridium::Utility::sizeToString(transfer.size).c_str();

	auto remainingTime = transfer.speed == 0 ? uint64_t(0) : static_cast<uint64_t>((transfer.size - transfer.bytes) / transfer.speed);
	// remainingTime to hh:mm:ss
	auto seconds = remainingTime % 60;
	auto minutes = remainingTime / 60 % 60;
	auto hours = remainingTime / 3600 % 24;

	at(4).first = remainingTime;
	at(4).second =
			QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0'));

	// ellapsedTime to hh:mm:ss
	auto now = duration_cast<std::chrono::seconds>(system_clock::now().time_since_epoch()).count();
	auto delta = now - _start_time;
	seconds = delta % 60;
	minutes = (delta / 60) % 60;
	hours = (delta / 3600) % 24;
	at(5).first = delta;
	at(5).second = QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0'));

	at(6).first = transfer.speed;
	at(6).second = (Iridium::Utility::sizeToString(transfer.speed) + "/s").c_str();
	at(7).first = transfer.speed_avg;
	at(7).second = (Iridium::Utility::sizeToString(transfer.speed_avg) + "/s").c_str();
}

void SyncRow::finish()
{
	at(1).first = int(100);
	at(1).second = 100;
	_state = Finished;
}

void SyncRow::error(const std::string &message)
{
	_state = Error;
	at(1).first = int(-1);
	at(1).second = -1;
}

uint32_t SyncRow::row() const { return _row; }

SyncRow::State SyncRow::state() const { return _state; }

bool SyncRow::compare(int collumn, Qt::SortOrder order, const SyncRow &other) const
{
	try
	{
		switch (collumn)
		{
			case 0:
				if (order == Qt::AscendingOrder)
					return at(0).second.toString() < other.at(0).second.toString();
				return at(0).second.toString() > other.at(0).second.toString();
			case 1:
				if (order == Qt::AscendingOrder)
					return any_cast<int>(at(1).first) < any_cast<int>(other.at(1).first);
				return any_cast<int>(at(1).first) > any_cast<int>(other.at(1).first);
			case 2:
				if (order == Qt::AscendingOrder)
					return at(2).second.toString() < other.at(2).second.toString();
				return at(2).second.toString() > other.at(2).second.toString();
			case 3:
				if (order == Qt::AscendingOrder)
					return any_cast<uint64_t>(at(3).first) < any_cast<uint64_t>(other.at(3).first);
				return any_cast<uint64_t>(at(3).first) > any_cast<uint64_t>(other.at(3).first);
			case 4:
				if (order == Qt::AscendingOrder)
					return any_cast<uint64_t>(at(4).first) < any_cast<uint64_t>(other.at(4).first);
				return any_cast<uint64_t>(at(4).first) > any_cast<uint64_t>(other.at(4).first);
			case 5:
				if (order == Qt::AscendingOrder)
					return any_cast<uint64_t>(at(5).first) < any_cast<uint64_t>(other.at(5).first);
				return any_cast<uint64_t>(at(5).first) > any_cast<uint64_t>(other.at(5).first);
			case 6:
				if (order == Qt::AscendingOrder)
					return any_cast<double_t>(at(6).first) < any_cast<double_t>(other.at(6).first);
				return any_cast<double_t>(at(6).first) > any_cast<double_t>(other.at(6).first);
			case 7:
				if (order == Qt::AscendingOrder)
					return any_cast<double_t>(at(7).first) < any_cast<double_t>(other.at(7).first);
				return any_cast<double_t>(at(7).first) > any_cast<double_t>(other.at(7).first);
			default:
				return false;
		}
	}
	catch (const std::bad_any_cast &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}
