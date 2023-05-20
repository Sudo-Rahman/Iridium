//
// Created by Rahman on 19/04/2023.
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
    _rclone->config(RemoteType::Pcloud, _remote_name->text().toStdString());
    _rclone->waitForStarted();
    _rclone->waitForFinished();
}
