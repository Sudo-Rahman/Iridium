//
// Created by sr-71 on 16/04/2023.
//

#include "OpenDriveRemoteConfigParamsFrame.hpp"

OpenDriveRemoteConfigParamsFrame::OpenDriveRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	OpenDriveRemoteConfigParamsFrame::createUi();
}

void OpenDriveRemoteConfigParamsFrame::createUi()
{
	m_userLineEdit = new QLineEdit(this);
	m_passwordLineEdit = new QLineEdit(this);

	m_userLineEdit->setPlaceholderText(tr("User"));
	m_passwordLineEdit->setPlaceholderText("********");

	m_passwordLineEdit->setEchoMode(QLineEdit::Password);


	m_formLayout->addRow(tr("Utilisateur"), m_userLineEdit);
	m_formLayout->addRow(tr("Mot de passe"), m_passwordLineEdit);

	RemoteConfigParamsFrame::createUi();
}

void OpenDriveRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	m_rclone->config(RemoteType::OpenDrive, m_remoteName->text().toStdString(), {
		"username=" + m_userLineEdit->text().toStdString(),
		"password=" + m_passwordLineEdit->text().toStdString()
	});
	m_rclone->waitForStarted();
	m_login->hide();
	m_cancel->show();
}