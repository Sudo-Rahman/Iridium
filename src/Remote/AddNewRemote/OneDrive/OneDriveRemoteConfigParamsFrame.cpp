//
// Created by Rahman on 14/04/2023.
//

#include "OneDriveRemoteConfigParamsFrame.hpp"

OneDriveRemoteConfigParamsFrame::OneDriveRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    OneDriveRemoteConfigParamsFrame::createUi();
}

void OneDriveRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    m_rclone->config(RemoteType::OneDrive, m_remoteName->text().toStdString());
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}


void OneDriveRemoteConfigParamsFrame::createUi()
{
    RemoteConfigParamsFrame::createUi();
}

bool OneDriveRemoteConfigParamsFrame::checkFields()
{
    return RemoteConfigParamsFrame::checkFields();
}
