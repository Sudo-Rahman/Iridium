//
// Created by Rahman on 29/04/2023.
//

#include "BoxRemoteConfigParamsFrame.hpp"

BoxRemoteConfigParamsFrame::BoxRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    BoxRemoteConfigParamsFrame::createUi();
}

void BoxRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::Box, _remote_name->text().toStdString());
    _rclone->waitForStarted();
    _login->hide();
    _cancel->show();
}
