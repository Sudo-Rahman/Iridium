#pragma once

#include <TreeFileViewContainer.hpp>
#include <ListRemoteWidget.hpp>
#include <QSplitter>
#include <iridium/process.hpp>
#include <TaskRowChild.hpp>
#include <RcloneFile.hpp>

class FileViewWidget final : public QWidget
{
	Q_OBJECT

	TreeFileViewContainer *_treeFileView1{}, *_treeFileView2{};
	QHBoxLayout * _layout{};

	QSplitter * _splitter{};

	QList<TreeFileItem *> _current_file_list{};

public:
	explicit FileViewWidget(QWidget * parent = nullptr);

	void changeRemote(const std::shared_ptr<remotes_selected>&);


signals:
	void taskAdded(const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
			   TaskRowParent::taskType type = TaskRowParent::Unknown);

};
