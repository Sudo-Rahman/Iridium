//
// Created by sr-71 on 29/04/2023.
//

#include "SmbRemoteConfigParamsFrame.hpp"
#include <QStyle>

SmbRemoteConfigParamsFrame::SmbRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    SmbRemoteConfigParamsFrame::createUi();
}

void SmbRemoteConfigParamsFrame::createUi()
{
    m_host = new QLineEdit(this);
    m_host->setPlaceholderText(tr("example.com"));
    m_formLayout->addRow(tr("Hôte :"), m_host);

    m_user = new QLineEdit(this);
    m_user->setPlaceholderText("toto");
    m_formLayout->addRow(tr("Utilisateur :"), m_user);

    m_password = new QLineEdit(this);
    m_password->setPlaceholderText("*********");
    m_password->setEchoMode(QLineEdit::Password);
    m_formLayout->addRow(tr("Mot de passe :"), m_password);

    m_domain = new QLineEdit(this);
    m_domain->setPlaceholderText("domain");
    m_domain->setText("WORKGROUP");
    auto action = m_domain->addAction(QIcon::fromTheme("reload"),
                                      QLineEdit::TrailingPosition);
    connect(action, &QAction::triggered, [this]() { m_domain->setText("WORKGROUP"); });
    action->setIconVisibleInMenu(true);
    m_formLayout->addRow(tr("Domaine :"), m_domain);

    m_port = new QSpinBox(this);
    m_port->setRange(1, 65535);
    m_port->setValue(445);
    m_formLayout->addRow(tr("Port :"), m_port);

    RemoteConfigParamsFrame::createUi();
}

void SmbRemoteConfigParamsFrame::addRemote()
{

    RemoteConfigParamsFrame::addRemote();
    if (not checkFields())
        return;
    m_rclone->config(RemoteType::Smb, m_remoteName->text().toStdString(),
                     {"host=" + m_host->text().toStdString(),
                      "user=" + m_user->text().toStdString(),
                      "pass=" + m_password->text().toStdString(),
                      "domain=" + m_domain->text().toStdString(),
                      "port=" + std::to_string(m_port->value())});
    m_rclone->waitForStarted();
    m_login->hide();
    m_cancel->show();
}

void SmbRemoteConfigParamsFrame::reset()
{
    RemoteConfigParamsFrame::reset();
    m_port->setValue(445);
    m_domain->setText("WORKGROUP");
}