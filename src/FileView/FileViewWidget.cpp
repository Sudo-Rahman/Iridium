//
// Created by sr-71 on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(const RemoteInfo& remoteInfo, QWidget *parent) : QWidget(parent)
{
	m_layout = new QHBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_treeFileView1 = new TreeFileViewContainer(std::make_shared<RemoteInfo>(remoteInfo), this);
	m_layout->addWidget(m_treeFileView1);

	m_treeFileView2 = new TreeFileViewContainer(std::make_shared<RemoteInfo>(RemoteInfo()), this);
	m_layout->addWidget(m_treeFileView2);

	changeRemote(std::make_shared<RemoteInfo>(RemoteInfo("/", RemoteType::LocalHardDrive)));

}

void FileViewWidget::changeRemote(const RemoteInfoPtr & remoteInfo)
{
	if (m_currentView == 0)
	{
		m_treeFileView2->changeRemote(remoteInfo);
		m_currentView = 1;
	}
	else
	{
		m_treeFileView1->changeRemote(remoteInfo);
		m_currentView = 0;
	}
}