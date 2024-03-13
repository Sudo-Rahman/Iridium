//
// Created by Rahman on 29/04/2023.
//

#include "SmbRemoteConfigParamsFrame.hpp"
#include <QStyle>

SmbRemoteConfigParamsFrame::SmbRemoteConfigParamsFrame(QWidget * parent) : RemoteConfigParamsFrame(parent)
{
	SmbRemoteConfigParamsFrame::createUi();
}

void SmbRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();

	_host = new RoundedLineEdit(this);
	_host->setPlaceholderText(tr("example.com"));
	_form_layout->addRow(tr("Hôte :"), _host);

	_user = new RoundedLineEdit(this);
	_user->setPlaceholderText("toto");
	_form_layout->addRow(tr("Utilisateur :"), _user);

	_password = new RoundedLineEdit(this);
	_password->setPlaceholderText("*********");
	_password->setEchoMode(QLineEdit::Password);
	_form_layout->addRow(tr("Mot de passe :"), _password);

	_domain = new RoundedLineEdit(this);
	_domain->setPlaceholderText("domain");
	_domain->setText("WORKGROUP");
	auto action = _domain->addAction(QIcon::fromTheme("reload"),
	                                 QLineEdit::TrailingPosition);
	connect(action, &QAction::triggered, [this]() { _domain->setText("WORKGROUP"); });
	action->setIconVisibleInMenu(true);
	_form_layout->addRow(tr("Domaine :"), _domain);

	_port = new QSpinBox(this);
	_port->setRange(1, 65535);
	_port->setValue(445);
	_form_layout->addRow(tr("Port :"), _port);
}

void SmbRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	using iridium::rclone::entity;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(entity::remote::remote_type_to_string(entity::remote::smb))
			.add_param(
				"host=" + _host->text().toStdString(),
				"user=" + _user->text().toStdString(),
				"pass=" + _password->text().toStdString(),
				"domain=" + _domain->text().toStdString(),
				"port=" + std::to_string(_port->value())
			)
			.execute();
}

void SmbRemoteConfigParamsFrame::reset()
{
	RemoteConfigParamsFrame::reset();
	_port->setValue(445);
	_domain->setText("WORKGROUP");
}
