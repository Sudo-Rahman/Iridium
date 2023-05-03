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
    m_rclone->config(RemoteType::Box, m_remoteName->text().toStdString());
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}
