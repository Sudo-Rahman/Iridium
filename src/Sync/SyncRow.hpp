//
// Created by sr-71 on 29/05/2024.
//

#pragma once
#include <any>
#include <LinearProgressBar.hpp>
#include <QStandardItem>
#include <RcloneFile.hpp>

class SyncRow : public std::vector<std::pair<std::any,QVariant>>
{
public:
	explicit SyncRow(const std::string &src, const std::string &dst, uint32_t row);

	void setTransferData(const ire::json_log::stats::transfer &transfer);

	void finish();

	void error(const std::string &message);

	uint32_t row() const;

	enum State
	{
		None,
		Syncing,
		Finished,
		Error
	};

	State state() const;

	bool compare(int collumn, Qt::SortOrder order,const SyncRow& other) const;

private:
	State _state{None};
	uint32_t _row;
	uint64_t _start_time{0};
};
