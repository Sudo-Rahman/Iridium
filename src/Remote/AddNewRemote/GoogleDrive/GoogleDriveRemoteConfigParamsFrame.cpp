//
// Created by rahman on 13/01/23.
//

#include <QMessageBox>
#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
        : RemoteConfigParamsFrame(parent)
{
    RemoteConfigParamsFrame::createUi();
}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::Drive, _remote_name->text().toStdString());
    _rclone->waitForStarted();
}