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
    _rclone->config(RemoteType::Sftp, _remote_name->text().toStdString(),
                    {"host=" + _host->text().toStdString(), "port=" + _port->text().toStdString(),
                     "user=" + _user->text().toStdString(), "pass=" + _password->text().toStdString()});
    _rclone->waitForFinished();
}

void SftpRemoteConfigParamsFrame::createUi()
{

    _host = new RoundedLineEdit(this);
    _host->setPlaceholderText(tr("exemple.com"));
    _form_layout->addRow(tr("Hôte : "), _host);


    _port = new QSpinBox(this);
    _port->setRange(1, 65535);
    _port->setValue(22);
    _form_layout->addRow(tr("Port : "), _port);

    _user = new RoundedLineEdit(this);
    _user->setPlaceholderText(tr("root"));
    _form_layout->addRow(tr("Utilisateur : "), _user);

    _password = new RoundedLineEdit(this);
    _password->setPlaceholderText(tr("********"));
    _password->setClearButtonEnabled(true);
    _password->setEchoMode(QLineEdit::Password);
    _form_layout->addRow(tr("Mot de passe : "), _password);

    RemoteConfigParamsFrame::createUi();
}

void SftpRemoteConfigParamsFrame::reset()
{
    RemoteConfigParamsFrame::reset();
    _port->setValue(22);
}