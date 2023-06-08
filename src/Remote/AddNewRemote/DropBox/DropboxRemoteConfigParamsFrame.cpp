//
// Created by Rahman on 14/04/2023.
//

#include "DropboxRemoteConfigParamsFrame.hpp"

DropboxRemoteConfigParamsFrame::DropboxRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    RemoteConfigParamsFrame::createUi();
}

void DropboxRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::Dropbox, _remote_name->text().toStdString());
    _rclone->waitForStarted();
}

