//
// Created by Rahman on 19/04/2023.
//

#include "PcloudRemoteConfigParamsFrame.hpp"


PcloudRemoteConfigParamsFrame::PcloudRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	RemoteConfigParamsFrame::createUi();
}

void PcloudRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(ire::remote::remote_type_to_string(ire::remote::pcloud))
			.execute();
}
