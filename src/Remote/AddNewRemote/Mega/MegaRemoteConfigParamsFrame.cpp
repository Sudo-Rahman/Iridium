//
// Created by Rahman on 15/04/2023.
//

#include "MegaRemoteConfigParamsFrame.hpp"

MegaRemoteConfigParamsFrame::MegaRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    MegaRemoteConfigParamsFrame::createUi();
}

void MegaRemoteConfigParamsFrame::createUi()
{
    _user = new QLineEdit(this);
    _user->setPlaceholderText("Email");
    _user->setPlaceholderText("toto@exemple.com");

    _password = new QLineEdit(this);
    _password->setPlaceholderText("*********");
    _password->setEchoMode(QLineEdit::Password);

    _form_layout->addRow(tr("Email"), _user);
    _form_layout->addRow(tr("Mot de passe"), _password);

    RemoteConfigParamsFrame::createUi();
}


void MegaRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::Mega, _remote_name->text().toStdString(), {
            "user=" + _user->text().toStdString(),
            "pass=" + _password->text().toStdString()
    });
    _rclone->waitForStarted();
    _login->hide();
    _cancel->show();
}
