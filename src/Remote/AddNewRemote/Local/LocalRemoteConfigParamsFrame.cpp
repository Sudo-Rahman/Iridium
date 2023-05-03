//
// Created by Rahman on 05/04/2023.
//

#include <QFileDialog>
#include <QMessageBox>
#include "LocalRemoteConfigParamsFrame.hpp"
#include <Settings.hpp>

LocalRemoteConfigParamsFrame::LocalRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
    LocalRemoteConfigParamsFrame::createUi();
}

void LocalRemoteConfigParamsFrame::createUi()
{

    auto *btn = new QPushButton(tr("Chemin du dossier"), this);

    auto *path = new QLineEdit(this);
    path->setPlaceholderText("/user/home");

    m_formLayout->addRow(btn, path);

    connect(btn, &QPushButton::clicked, this, [this, path]()
    {
        auto result = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
        if (!result.isEmpty())
        {
            m_path = result;
            path->setText(result);
        }
    });

    connect(path, &QLineEdit::textChanged, this, [this, path](const QString &text)
    {
        m_path = text;
        if (QFileInfo::exists(text))
        {
            path->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
            path->setToolTip("");
        } else
        {
            path->setStyleSheet("border: 1px solid red; border-radius: 5px;");
            path->setToolTip(tr("Le chemin n’existe pas !"));
        }
    });

    RemoteConfigParamsFrame::createUi();

    m_login->setText(tr("Ajouter"));

}

void LocalRemoteConfigParamsFrame::addRemote()
{
    if (QFileInfo::exists(m_path))
    {
        Settings::addLocalRemote(
                {m_path.toStdString(), RemoteType::LocalHardDrive, m_remoteName->text().toStdString()});
        emit remoteAdded();
        QMessageBox::information(this, tr("Ajout d’un nouveau remote local"),
                                 tr("Le remote %1 a été ajouté avec succès !").QString::arg(m_remoteName->text()));
        clearAllFields();

    } else
    {
        m_messLabel->setText(tr("Le chemin n’existe pas !"));
        m_messLabel->show();
    }
}
