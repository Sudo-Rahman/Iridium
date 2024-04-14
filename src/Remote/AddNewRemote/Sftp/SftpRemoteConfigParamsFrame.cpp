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
    using iridium::rclone::entity;
    _process->config_create().name(_remote_name->text().toStdString())
            .type(ire::remote::remote_type_to_string(ire::remote::sftp))
            .add_param(
                    "host=" + _host->text().toStdString(),
                    "port=" + std::to_string(_port->value()),
                    "user=" + _user->text().toStdString(),
                    "pass=" + _password->text().toStdString()
            )
            .execute();
}

void SftpRemoteConfigParamsFrame::createUi()
{
    RemoteConfigParamsFrame::createUi();

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
}

void SftpRemoteConfigParamsFrame::reset()
{
    RemoteConfigParamsFrame::reset();
    _port->setValue(22);
}