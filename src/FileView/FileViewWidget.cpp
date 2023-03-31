//
// Created by sr-71 on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(RemoteInfo remoteInfo, QWidget *parent) : QWidget(parent), m_remoteInfo1(std::move(remoteInfo))
{
	m_layout = new QHBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_treeFileView1 = new TreeFileView(m_remoteInfo1, this);
	m_layout->addWidget(m_treeFileView1);

	m_treeFileView2 = new TreeFileView(RemoteInfo(), this);
	m_layout->addWidget(m_treeFileView2);

	changeRemote2(RemoteInfo("maison",RemoteType::LocalHardDrive,"/"));

}

void FileViewWidget::changeRemote1(RemoteInfo info)
{
	m_remoteInfo1 = std::move(info);
	m_treeFileView1->changeRemote(m_remoteInfo1);
}

void FileViewWidget::changeRemote2(RemoteInfo info)
{
	m_remoteInfo2 = std::move(info);
	m_treeFileView2->changeRemote(m_remoteInfo2);
}
