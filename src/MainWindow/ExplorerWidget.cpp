//
// Created by sr-71 on 09/05/2023.
//

#include "ExplorerWidget.hpp"
#include <RcloneFile.hpp>


ExplorerWidget::ExplorerWidget(QWidget * parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_layout = new QHBoxLayout(this);
	_layout->setContentsMargins(5, 0, 5, 5);

	_list_remoteWidget = new ListRemoteWidget(this);
	_layout->addWidget(_list_remoteWidget);

	_splitter = new QSplitter(this);
	_splitter->setOrientation(Qt::Vertical);
	_splitter->setHandleWidth(15);
	// no collapsible first widget
	_layout->addWidget(_splitter);

	_file_viewWidget = new FileViewWidget(this);
	_splitter->addWidget(_file_viewWidget);
	_file_viewWidget->changeRemote(_list_remoteWidget->remoteSelected());

	_task_treeView = new TaskTreeView(this);
	_splitter->addWidget(_task_treeView);

	_splitter->setCollapsible(0, false);
	_splitter->setCollapsible(1, true);
	// collapse second widget
	_splitter->setSizes({10000, 0});

	connectSignals();
}

void ExplorerWidget::connectSignals()
{
	connect(_list_remoteWidget, &ListRemoteWidget::remoteClicked, this,
	        [this](const std::shared_ptr<remotes_selected>& remotes) { _file_viewWidget->changeRemote(remotes); });
	connect(_file_viewWidget, &FileViewWidget::taskAdded, this,
	        [this](const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
	               TaskRowParent::taskType type)
	        {
		        if (_splitter->sizes()[1] == 0)
			        _splitter->setSizes({1000, 400});
		        _task_treeView->addTask(src, dst, rclone, type);
	        });
}
