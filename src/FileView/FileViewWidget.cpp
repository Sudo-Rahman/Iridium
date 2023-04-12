//
// Created by sr-71 on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(QWidget *parent) : QWidget(parent)
{
	m_layout = new QHBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Horizontal);
	m_layout->addWidget(m_splitter);

	m_treeFileView1 = new TreeFileViewContainer(this);
	m_splitter->addWidget(m_treeFileView1);

	m_treeFileView2 = new TreeFileViewContainer(this);
	m_splitter->addWidget(m_treeFileView2);

	connect(m_treeFileView1->treeFileView(), &TreeFileView::fileCopied, this, [this](const QList<TreeFileItem *> &lst)
	{ m_currentFileList = lst; });
	connect(m_treeFileView2->treeFileView(), &TreeFileView::fileCopied, this, [this](const QList<TreeFileItem *> &lst)
	{ m_currentFileList = lst; });
	connect(m_treeFileView1->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
	{
		if (m_currentFileList.empty())
			return;
		QList<RcloneFilePtr> lst;
		for (auto item: m_currentFileList)
			lst << item->getFile();
		m_treeFileView1->treeFileView()->copyto(m_currentFileList);
	});
	connect(m_treeFileView2->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
	{
		if (m_currentFileList.empty())
			return;
		QList<RcloneFilePtr> lst;
		for (auto item: m_currentFileList)
			lst << item->getFile();
		m_treeFileView2->treeFileView()->copyto(m_currentFileList);
	});
}

void FileViewWidget::changeRemote(const std::shared_ptr<remotes_selected> &remotes)
{
	if (remotes->first->remoteInfo().get() != m_treeFileView1->treeFileView()->remoteInfo().get())
	{
		m_treeFileView1->treeFileView()->changeRemote(remotes->first->remoteInfo());
	}
	if (remotes->second->remoteInfo().get() != m_treeFileView2->treeFileView()->remoteInfo().get())
	{
		m_treeFileView2->treeFileView()->changeRemote(remotes->second->remoteInfo());
	}
}