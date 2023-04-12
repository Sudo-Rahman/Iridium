//
// Created by sr-71 on 08/04/2023.
//

#include "SftpRemoteConfigParamsFrame.hpp"

SftpRemoteConfigParamsFrame::SftpRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	SftpRemoteConfigParamsFrame::createUi();
}

void SftpRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	m_rclone->config(RemoteType::Sftp, m_remoteName->text().toStdString(),
					 {"host=" + m_host->text().toStdString(), "port=" + m_port->text().toStdString(),
					  "user=" + m_user->text().toStdString(), "pass=" + m_password->text().toStdString()});
	m_rclone->waitForStarted();
	m_rclone->waitForFinished();
}

void SftpRemoteConfigParamsFrame::createUi()
{

	m_host = new QLineEdit(this);
	m_host->setPlaceholderText(tr("exemple.com"));
	connecLineEdit(m_host);
	m_formLayout->addRow(tr("Hôte : "), m_host);


	m_port = new QSpinBox(this);
	m_port->setRange(1, 65535);
	m_port->setValue(22);
	m_formLayout->addRow(tr("Port : "), m_port);

	m_user = new QLineEdit(this);
	m_user->setPlaceholderText(tr("root"));
	connecLineEdit(m_user);
	m_formLayout->addRow(tr("Utilisateur : "), m_user);

	m_password = new QLineEdit(this);
	m_password->setPlaceholderText(tr("********"));
	m_password->setClearButtonEnabled(true);
	m_password->setEchoMode(QLineEdit::Password);
	connecLineEdit(m_password);
	m_formLayout->addRow(tr("Mot de passe : "), m_password);

	RemoteConfigParamsFrame::createUi();
}

bool SftpRemoteConfigParamsFrame::checkFields()
{
	bool ok;
	ok = RemoteConfigParamsFrame::checkFields();
	if (m_host->text().isEmpty())
	{
		m_host->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		ok = false;
	}
	if (m_user->text().isEmpty())
	{
		m_user->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		ok = false;
	}
	if (m_password->text().isEmpty())
	{
		m_password->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		ok = false;
	}
	return ok;
}
