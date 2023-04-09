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
	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	m_layout->addLayout(layout);

	layout->addWidget(new QLabel(tr("Hôte : "), this));
	m_host = new QLineEdit(this);
	m_host->setPlaceholderText(tr("exemple.com"));
	connecLineEdit(m_host);
	layout->addWidget(m_host);

	auto layout2 = new QHBoxLayout;
	m_layout->addLayout(layout2);
	layout2->setContentsMargins(0, 0, 0, 0);

	layout2->addWidget(new QLabel(tr("Port : "), this));
	m_port = new QSpinBox(this);
	m_port->setRange(1, 65535);
	m_port->setValue(22);
	layout2->addWidget(m_port);

	auto layout3 = new QHBoxLayout;
	m_layout->addLayout(layout3);
	layout3->setContentsMargins(0, 0, 0, 0);

	layout3->addWidget(new QLabel(tr("Utilisateur : "), this));
	m_user = new QLineEdit(this);
	m_user->setPlaceholderText(tr("root"));
	connecLineEdit(m_user);
	layout3->addWidget(m_user);

	auto layout4 = new QHBoxLayout;
	m_layout->addLayout(layout4);
	layout4->setContentsMargins(0, 0, 0, 0);

	layout4->addWidget(new QLabel(tr("Mot de passe : "), this));
	m_password = new QLineEdit(this);
	m_password->setPlaceholderText(tr("********"));
	m_password->setClearButtonEnabled(true);
	m_password->setEchoMode(QLineEdit::Password);
	connecLineEdit(m_password);
	layout4->addWidget(m_password);

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
