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
    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setMinimum(1);
    m_portSpinBox->setMaximum(65535);
    m_portSpinBox->setValue(21);
    m_portSpinBox->setSingleStep(1);
    m_portSpinBox->setToolTip("Port number");

    m_userLineEdit = new QLineEdit(this);
    m_userLineEdit->setPlaceholderText("Utilisateur");
    m_userLineEdit->setToolTip("Utilisateur");

    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setClearButtonEnabled(true);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setPlaceholderText("Password");
    m_passwordLineEdit->setToolTip("********");

    m_hostLineEdit = new QLineEdit(this);
    m_hostLineEdit->setPlaceholderText("Host");
    m_hostLineEdit->setToolTip("ftp.example.com");

    m_tls = new QComboBox(this);
    m_tls->setToolTip("False");
    m_tls->addItem("False");
    m_tls->addItem("True");
    m_tls->setToolTip("True");

    m_formLayout->addRow(tr("Hote : "), m_hostLineEdit);
    m_formLayout->addRow(tr("Port : "), m_portSpinBox);
    m_formLayout->addRow(tr("Utilisateur : "), m_userLineEdit);
    m_formLayout->addRow(tr("Mot de passe : "), m_passwordLineEdit);
    m_formLayout->addRow(tr("TLS : "), m_tls);


    connect(m_tls, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
    {
        if (index == 0)
            m_portSpinBox->setValue(21);
        else
            m_portSpinBox->setValue(990);
    });

    RemoteConfigParamsFrame::createUi();

}


void FtpRemoteConfigParamsFrame::addRemote()
{
    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    m_rclone->config(RemoteType::Ftp, m_remoteName->text().toStdString(), {
            "host=" + m_hostLineEdit->text().toStdString(),
            "port=" + std::to_string(m_portSpinBox->value()),
            "user=" + m_userLineEdit->text().toStdString(),
            "pass=" + m_passwordLineEdit->text().toStdString(),
            "tls=" + m_tls->currentText().toLower().toStdString()});
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}

void FtpRemoteConfigParamsFrame::reset()
{
    m_portSpinBox->setValue(21);
    m_tls->setCurrentIndex(0);
    RemoteConfigParamsFrame::reset();
}