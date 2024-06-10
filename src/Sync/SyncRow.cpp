//
// Created by sr-71 on 29/05/2024.
//

#include "SyncRow.hpp"

#include "Utility/Utility.hpp"

SyncRow::SyncRow(const std::string &src, const std::string &dst, uint32_t row) : _row(row)
{
	push_back(src.c_str());
	push_back(0);
	push_back(dst.c_str());
	push_back("--");
	push_back("--");
	push_back("--");
	push_back("--");
	push_back("--");
}

using namespace std::chrono;

void SyncRow::setTransferData(const ire::json_log::stats::transfer &transfer)
{
	if (_state == None) { _start_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count(); }
	_state = Syncing;
	at(1) = transfer.percentage;

	at(3) = Iridium::Utility::sizeToString(transfer.size).c_str();

	auto remainingTime = static_cast<double_t>(transfer.size - transfer.bytes) / transfer.speed;
	// remainingTime to hh:mm:ss
	auto seconds = (uint64_t)remainingTime % 60;
	auto minutes = ((uint64_t)remainingTime / 60) % 60;
	auto hours = ((uint64_t)remainingTime / 3600) % 24;

	at(4) =
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
	at(5) = QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0'));

	at(6) = (Iridium::Utility::sizeToString(transfer.speed) + "/s").c_str();
	at(7) = (Iridium::Utility::sizeToString(transfer.speed_avg) + "/s").c_str();
}

void SyncRow::finish()
{
	at(1) = 100;
	_state = Finished;
}

void SyncRow::error(const std::string &message)
{
	_state = Error;
	at(1) = -1;
}

uint32_t SyncRow::row() const { return _row; }

SyncRow::State SyncRow::state() const { return _state; }
