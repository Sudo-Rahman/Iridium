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
    _rclone->config(RemoteType::OneDrive, _remote_name->text().toStdString());
    _rclone->waitForStarted();
}


void OneDriveRemoteConfigParamsFrame::createUi()
{
    RemoteConfigParamsFrame::createUi();
}

bool OneDriveRemoteConfigParamsFrame::checkFields()
{
    return RemoteConfigParamsFrame::checkFields();
}
