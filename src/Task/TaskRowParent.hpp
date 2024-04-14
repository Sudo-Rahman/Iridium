//
// Created by Rahman on 31/03/2024.
//
#pragma once

#include <TaskRow.hpp>
#include <iridium/entities.hpp>

class TaskRowParent : public TaskRow
{
public:
	enum taskType
	{
		Copy,
		Move,
		Mkdir,
		Delete,
		Rename,
		Unknown
	};

	TaskRowParent(const RcloneFile& src, const RcloneFile& dest, const ire::json_log& data, taskType taskType);

	TaskRowParent(const RcloneFile& file, const ire::json_log& data, taskType taskType);

	void updateData(const boost::variant2::variant<ire::json_log, ire::json_log::stats::transfer>&) override;

	void updateData();

	void finished() override;

private:
	RcloneFileUPtr _src{}, _dest{};

	uint64_t _size{};

	ire::json_log _data{};

	std::vector<double_t> _avg{};

	taskType _taskType{Unknown};

	void init() override;

	void setSpeed();
};
