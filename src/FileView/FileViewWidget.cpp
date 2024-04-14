//
// Created by Rahman on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(QWidget * parent) : QWidget(parent)
{
	_layout = new QHBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	_layout->setContentsMargins(0, 0, 0, 0);

	_splitter = new QSplitter(this);
	_splitter->setOrientation(Qt::Horizontal);
	_layout->addWidget(_splitter);

	_treeFileView1 = new TreeFileViewContainer(this);
	_splitter->addWidget(_treeFileView1);

	_treeFileView2 = new TreeFileViewContainer(this);
	_splitter->addWidget(_treeFileView2);

	connect(_treeFileView1->treeFileView(), &TreeFileView::taskAdded, this,
	        [this](const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
	        	const TaskRowParent::taskType& type)
	        {
		        emit taskAdded(src, dst, rclone, type);
	        });

	connect(_treeFileView2->treeFileView(), &TreeFileView::taskAdded, this,
	        [this](const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
	               const TaskRowParent::taskType& type)
	        {
		        emit taskAdded(src, dst, rclone, type);
	        });
}

void FileViewWidget::changeRemote(const std::shared_ptr<remotes_selected>& remotes)
{
	_current_file_list.clear();

	if (remotes->first not_eq nullptr)
	{
		if (remotes->first->remoteInfo().get() != _treeFileView1->treeFileView()->remoteInfo().get())
		{
			_treeFileView1->treeFileView()->changeRemote(remotes->first->remoteInfo());
		}
	}
	else
		_treeFileView1->treeFileView()->changeRemote(nullptr);


	if (remotes->second not_eq nullptr)
	{
		if (remotes->second->remoteInfo().get() != _treeFileView2->treeFileView()->remoteInfo().get())
		{
			_treeFileView2->treeFileView()->changeRemote(remotes->second->remoteInfo());
		}
	}
	else
		_treeFileView2->treeFileView()->changeRemote(nullptr);
}
