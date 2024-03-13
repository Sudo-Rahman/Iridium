//
// Created by Rahman on 15/04/2023.
//

#include "MegaRemoteConfigParamsFrame.hpp"

MegaRemoteConfigParamsFrame::MegaRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	MegaRemoteConfigParamsFrame::createUi();
}

void MegaRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();

	_user = new RoundedLineEdit(this);
	_user->setPlaceholderText("Email");
	_user->setPlaceholderText("toto@exemple.com");

	_password = new RoundedLineEdit(this);
	_password->setPlaceholderText("*********");
	_password->setEchoMode(QLineEdit::Password);

	_form_layout->addRow(tr("Email"), _user);
	_form_layout->addRow(tr("Mot de passe"), _password);
}


void MegaRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	iridium::rclone::process().config_create().name(_remote_name->text().toStdString())
			.type(entity::remote::remote_type_to_string(entity::remote::mega))
			.add_param(
				"user=" + _user->text().toStdString(),
				"pass=" + _password->text().toStdString()
			)
			.execute();
}
