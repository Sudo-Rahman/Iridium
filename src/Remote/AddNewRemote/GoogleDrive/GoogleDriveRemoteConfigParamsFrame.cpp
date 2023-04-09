//
// Created by rahman on 13/01/23.
//

#include <QMessageBox>
#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
	: RemoteConfigParamsFrame(parent)
{
	GoogleDriveRemoteConfigParamsFrame::createUi();
}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{

	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	m_rclone->config(RemoteType::Drive, m_remoteName->text().toStdString());
	m_rclone->waitForStarted();
	logInBtn->hide();
	cancelBtn->show();
}

void GoogleDriveRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();
}

bool GoogleDriveRemoteConfigParamsFrame::checkFields()
{
	return RemoteConfigParamsFrame::checkFields();
}
