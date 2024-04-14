//
// Created by Rahman on 12/04/2023.
//

#pragma once

#include <iridium/entities.hpp>

#include "TaskRow.hpp"

class TaskRowChild : public TaskRow
{
public:
	explicit TaskRowChild(const RcloneFile& src, const RcloneFile& dest, const ire::json_log::stats::transfer&);

	void finished() override;

	void updateData(const boost::variant2::variant<ire::json_log, ire::json_log::stats::transfer>&) override;

private:
	RcloneFile _src{}, _dest{};

	uint64_t _size{};

	ire::json_log::stats::transfer _data{};

	std::vector<double_t> _avg{};

	void init() override;

	void setSpeed() const;

	void updateDataChild();
};
