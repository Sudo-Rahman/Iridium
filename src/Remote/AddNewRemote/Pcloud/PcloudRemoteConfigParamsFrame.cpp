//
// Created by sr-71 on 19/04/2023.
//

#include "PcloudRemoteConfigParamsFrame.hpp"

PcloudRemoteConfigParamsFrame::PcloudRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	PcloudRemoteConfigParamsFrame::createUi();
}

void PcloudRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	m_rclone->config(RemoteType::Pcloud, m_remoteName->text().toStdString());
	m_rclone->waitForStarted();
	m_rclone->waitForFinished();
}
