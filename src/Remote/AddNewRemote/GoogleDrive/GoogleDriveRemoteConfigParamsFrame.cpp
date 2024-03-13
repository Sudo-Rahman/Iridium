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
    using iridium::rclone::entity;
    _process->config_create().name(_remote_name->text().toStdString())
            .type(entity::remote::remote_type_to_string(entity::remote::google_drive))
            .execute();
}