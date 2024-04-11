//
// Created by Rahman on 12/04/2023.
//

#pragma once

#include <RcloneFile.hpp>
#include "TaskRowChild.hpp"
#include "TaskRowParent.hpp"

struct Tasks
{
	std::unique_ptr<TaskRowParent> parent;
	ir::process_ptr rclone;
	std::map<size_t, std::unique_ptr<TaskRowChild>> children;
};

class TaskTreeView : public QTreeView
{
	Q_OBJECT

	static std::unordered_map<size_t, Tasks> _tasks;

	QStandardItemModel * _model{};

	void cancelTask();

public:
	explicit TaskTreeView(QWidget * parent = nullptr);

	void addTask(const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
	             TaskRowParent::taskType type = TaskRowParent::Unknown);

};
