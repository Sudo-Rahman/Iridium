//
// Created by Rahman on 14/04/2023.
//

#include "FtpRemoteConfigParamsFrame.hpp"

FtpRemoteConfigParamsFrame::FtpRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    FtpRemoteConfigParamsFrame::createUi();
}

void FtpRemoteConfigParamsFrame::createUi()
{
    _port = new QSpinBox(this);
    _port->setMinimum(1);
    _port->setMaximum(65535);
    _port->setValue(21);
    _port->setSingleStep(1);
    _port->setToolTip("Port number");

    _user = new RoundedLineEdit(this);
    _user->setPlaceholderText("Utilisateur");
    _user->setToolTip("Utilisateur");

    _password = new RoundedLineEdit(this);
    _password->setClearButtonEnabled(true);
    _password->setEchoMode(QLineEdit::Password);
    _password->setPlaceholderText("Password");
    _password->setToolTip("********");

    _host = new RoundedLineEdit(this);
    _host->setPlaceholderText("Host");
    _host->setToolTip("ftp.example.com");

    _tls = new QComboBox(this);
    _tls->setToolTip("False");
    _tls->addItem("False");
    _tls->addItem("True");
    _tls->setToolTip("True");

    _form_layout->addRow(tr("Hote : "), _host);
    _form_layout->addRow(tr("Port : "), _port);
    _form_layout->addRow(tr("Utilisateur : "), _user);
    _form_layout->addRow(tr("Mot de passe : "), _password);
    _form_layout->addRow(tr("TLS : "), _tls);


    connect(_tls, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
    {
        if (index == 0)
            _port->setValue(21);
        else
            _port->setValue(990);
    });

    RemoteConfigParamsFrame::createUi();

}


void FtpRemoteConfigParamsFrame::addRemote()
{
    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    _rclone->config(RemoteType::Ftp, _remote_name->text().toStdString(), {
            "host=" + _host->text().toStdString(),
            "port=" + std::to_string(_port->value()),
            "user=" + _user->text().toStdString(),
            "pass=" + _password->text().toStdString(),
            "tls=" + _tls->currentText().toLower().toStdString()});
    _rclone->waitForStarted();
    _login->hide();
    _cancel->show();
}

void FtpRemoteConfigParamsFrame::reset()
{
    RemoteConfigParamsFrame::reset();
    _port->setValue(21);
    _tls->setCurrentIndex(0);
}