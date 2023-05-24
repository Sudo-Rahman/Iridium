//
// Created by Rahman on 16/04/2023.
//

#include "OpenDriveRemoteConfigParamsFrame.hpp"

OpenDriveRemoteConfigParamsFrame::OpenDriveRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    OpenDriveRemoteConfigParamsFrame::createUi();
}

void OpenDriveRemoteConfigParamsFrame::createUi()
{
    _user = new RoundedLineEdit(this);
    _password = new RoundedLineEdit(this);

    _user->setPlaceholderText("Toto");
    _password->setPlaceholderText("********");

    _password->setEchoMode(QLineEdit::Password);


    _form_layout->addRow(tr("Utilisateur"), _user);
    _form_layout->addRow(tr("Mot de passe"), _password);

    RemoteConfigParamsFrame::createUi();
}

void OpenDriveRemoteConfigParamsFrame::addRemote()
{
    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::OpenDrive, _remote_name->text().toStdString(), {
            "username=" + _user->text().toStdString(),
            "password=" + _password->text().toStdString()
    });
    _rclone->waitForStarted();
    _login->hide();
    _cancel->show();
}