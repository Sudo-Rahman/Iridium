//
// Created by Rahman on 14/04/2023.
//

#include "DropboxRemoteConfigParamsFrame.hpp"

DropboxRemoteConfigParamsFrame::DropboxRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    DropboxRemoteConfigParamsFrame::createUi();
}

void DropboxRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    m_rclone->config(RemoteType::Dropbox, m_remoteName->text().toStdString());
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}

