//
// Created by Rahman on 14/04/2023.
//

#include "DropboxRemoteConfigParamsFrame.hpp"

DropboxRemoteConfigParamsFrame::DropboxRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	RemoteConfigParamsFrame::createUi();
}

void DropboxRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(entity::remote::remote_type_to_string(entity::remote::dropbox))
			.execute();
}
