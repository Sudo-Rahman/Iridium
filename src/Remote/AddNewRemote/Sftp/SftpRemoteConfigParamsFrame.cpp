//
// Created by Rahman on 08/04/2023.
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
    m_formLayout->addRow(tr("Hôte : "), m_host);


    m_port = new QSpinBox(this);
    m_port->setRange(1, 65535);
    m_port->setValue(22);
    m_formLayout->addRow(tr("Port : "), m_port);

    m_user = new QLineEdit(this);
    m_user->setPlaceholderText(tr("root"));
    m_formLayout->addRow(tr("Utilisateur : "), m_user);

    m_password = new QLineEdit(this);
    m_password->setPlaceholderText(tr("********"));
    m_password->setClearButtonEnabled(true);
    m_password->setEchoMode(QLineEdit::Password);
    m_formLayout->addRow(tr("Mot de passe : "), m_password);

    RemoteConfigParamsFrame::createUi();
}

void SftpRemoteConfigParamsFrame::reset()
{
    RemoteConfigParamsFrame::reset();
    m_port->setValue(22);
}