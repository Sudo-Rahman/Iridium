//
// Created by rahman on 13/01/23.
//

#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
        : RemoteConfigParamsFrame(parent)
{
    rclone = new Rclone;
    connect(rclone, &Rclone::exitCode, this,[this](int exit){
        if(exit){
            QMessageBox
        }
    });
    createUi();

}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{
    if (rclone not_eq nullptr and rclone->state() == Rclone::Running)
    {
        rclone->terminate();
        rclone->waitForFinished(10);
    }
    rclone->config(RemoteType::Drive, {"tg"});
}

void GoogleDriveRemoteConfigParamsFrame::createUi()
{

    RemoteConfigParamsFrame::createUi();

}
