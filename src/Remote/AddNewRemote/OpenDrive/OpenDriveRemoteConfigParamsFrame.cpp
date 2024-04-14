//
// Created by Rahman on 16/04/2023.
//

#include "OpenDriveRemoteConfigParamsFrame.hpp"

OpenDriveRemoteConfigParamsFrame::OpenDriveRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	OpenDriveRemoteConfigParamsFrame::createUi();
}

void OpenDriveRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();

	_user = new RoundedLineEdit(this);
	_password = new RoundedLineEdit(this);

	_user->setPlaceholderText("Toto");
	_password->setPlaceholderText("********");

	_password->setEchoMode(QLineEdit::Password);


	_form_layout->addRow(tr("Utilisateur"), _user);
	_form_layout->addRow(tr("Mot de passe"), _password);
}

void OpenDriveRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(ire::remote::remote_type_to_string(ire::remote::opendrive))
			.add_param(
				"username=" + _user->text().toStdString(),
				"password=" + _password->text().toStdString()
			)
			.execute();
}
