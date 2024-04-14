//
// Created by Rahman on 29/04/2023.
//

#include "BoxRemoteConfigParamsFrame.hpp"

BoxRemoteConfigParamsFrame::BoxRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	RemoteConfigParamsFrame::createUi();
}

void BoxRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(ire::remote::remote_type_to_string(ire::remote::box))
			.execute();
}
