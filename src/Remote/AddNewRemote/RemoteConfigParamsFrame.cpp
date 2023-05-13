//
// Created by rahman on 13/01/23.
//

#include <QPainter>
#include <QMessageBox>

#include "RemoteConfigParamsFrame.hpp"

using namespace std;

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{

    m_layout = new QVBoxLayout(this);

    m_formLayout = new QFormLayout;
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_formLayout->setFormAlignment(Qt::AlignTop);
    m_formLayout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(m_formLayout);

    m_remoteName = new QLineEdit(this);
    m_formLayout->addRow(tr("Nom : "), m_remoteName);
}

/**
 * @brief creation of the ui
 */
void RemoteConfigParamsFrame::createUi()
{
    auto *tmpwidlay = new QHBoxLayout;

    m_login = new QPushButton(tr("Se connecter"), this);
    m_login->setDefault(true);
    m_login->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    tmpwidlay->addWidget(m_login, Qt::AlignTop);
    tmpwidlay->setAlignment(m_login, Qt::AlignTop);

    m_cancel = new QPushButton(tr("Annuler"), this);
    m_cancel->hide();
    m_cancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tmpwidlay->addWidget(m_cancel, Qt::AlignTop);
    tmpwidlay->setAlignment(m_cancel, Qt::AlignTop);

    m_layout->addLayout(tmpwidlay);

    connect(m_login, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);

    connect(m_cancel, &QPushButton::clicked, this, [this]()
    {
        if (m_rclone->state() == Rclone::Running)
            m_rclone->kill();
        m_cancel->hide();
        m_login->show();
    });

    m_messLabel = new QLabel(this);
    m_messLabel->setText(tr("Les champs en rouge sont obligatoires !"));
    m_messLabel->hide();
    m_messLabel->setAutoFillBackground(true);
    QPalette p;
    p.setColor(QPalette::WindowText, Qt::red);
    m_messLabel->setPalette(p);
    m_layout->addWidget(m_messLabel, Qt::AlignTop);
    m_layout->setAlignment(m_messLabel, Qt::AlignTop);

    m_remoteName->setStyleSheet("border: 1px solid gray; border-radius: 5px;");

}

/**
 * @brief add a new remote  function called when the user click on the login button
 */
void RemoteConfigParamsFrame::addRemote()
{
    if (m_remoteName->text().isEmpty())
    {
        m_remoteName->setStyleSheet("border: 1px solid red; border-radius: 5px;");
        m_messLabel->show();
        m_messLabel->setText(tr("Les champs en rouge sont obligatoires !"));
        return;
    }

    m_rclone = RcloneManager::get();
    connect(m_rclone.get(), &Rclone::finished, this, [this](int exit)
    {
        if (exit == 0)
        {
            emit remoteAdded();
            QMessageBox::information(this, tr("Succès"),
                                     tr("Le disque %1 a été ajouté avec succès").arg(m_remoteName->text()));
            clearAllFields();
            m_cancel->hide();
            m_login->show();
        } else
        {
            auto msgBox = QMessageBox();
            msgBox.setWindowTitle(tr("Erreur"));
            msgBox.setText(tr("Une erreur est survenue lors de la configuration du serveur distant !"));
            msgBox.setDetailedText(m_rclone->readAllError().back().c_str());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
    });

    auto rclone_liste_remote = RcloneManager::get();
    rclone_liste_remote->listRemotes();
    rclone_liste_remote->waitForStarted();
    rclone_liste_remote->waitForFinished();
    m_lstRemote = rclone_liste_remote->getData();

}

/**
 * @brief check if all fields are filled
 * @return true if all fields are filled
 */
bool RemoteConfigParamsFrame::checkFields()
{
    bool ok = true;
    if (m_lstRemote.contains(m_remoteName->text().toStdString()))
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Le nom du disque est déjà utilisé !"));
        return false;
    }
    for (auto &field: findChildren<QLineEdit *>())
    {
        if (field->text().isEmpty())
        {
            field->setStyleSheet("border: 1px solid red; border-radius: 5px;");
            m_messLabel->show();
            m_messLabel->setText(tr("Les champs en rouge sont obligatoires !"));
            ok = false;
        }
    }
    return ok;
}

/**
 * @brief clear all fields
 */
void RemoteConfigParamsFrame::clearAllFields()
{
    for (auto &field: findChildren<QLineEdit *>())
    {
        field->clear();
        field->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
    }
}

/**
 * @brief reset the frame
 */
void RemoteConfigParamsFrame::reset()
{
    clearAllFields();
    m_messLabel->hide();
}