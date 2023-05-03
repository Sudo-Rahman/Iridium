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
    m_login->hide();
    m_cancel->show();
}