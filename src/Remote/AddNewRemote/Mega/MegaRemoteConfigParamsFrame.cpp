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
    m_userLineEdit = new QLineEdit(this);
    m_userLineEdit->setPlaceholderText("Email");
    m_userLineEdit->setPlaceholderText("toto@exemple.com");

    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setPlaceholderText("*********");
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);

    m_formLayout->addRow(tr("Email"), m_userLineEdit);
    m_formLayout->addRow(tr("Mot de passe"), m_passwordLineEdit);

    RemoteConfigParamsFrame::createUi();
}


void MegaRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    m_rclone->config(RemoteType::Mega, m_remoteName->text().toStdString(), {
            "user=" + m_userLineEdit->text().toStdString(),
            "pass=" + m_passwordLineEdit->text().toStdString()
    });
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}
