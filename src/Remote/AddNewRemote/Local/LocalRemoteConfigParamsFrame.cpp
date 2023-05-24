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

    auto *path = new RoundedLineEdit(this);
    path->setPlaceholderText("/user/home");

    _form_layout->addRow(btn, path);

    connect(btn, &QPushButton::clicked, this, [this, path]()
    {
        auto result = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
        if (!result.isEmpty())
        {
            _path = result;
            path->setText(result);
        }
    });

    connect(path, &QLineEdit::textChanged, this, [this, path](const QString &text)
    {
        _path = text;
        if (QFileInfo::exists(text))
        {
            path->normalBorder();
            path->setToolTip("");
        } else
        {
            path->redBorder();
            path->setToolTip(tr("Le chemin n’existe pas !"));
        }
    });

    RemoteConfigParamsFrame::createUi();

    _login->setText(tr("Ajouter"));

}

void LocalRemoteConfigParamsFrame::addRemote()
{
    if (QFileInfo::exists(_path))
    {
        Settings::addLocalRemote(
                {_path.toStdString(), RemoteType::LocalHardDrive, _remote_name->text().toStdString()});
        emit remoteAdded();
        QMessageBox::information(this, tr("Ajout d’un nouveau remote local"),
                                 tr("Le remote %1 a été ajouté avec succès !").QString::arg(_remote_name->text()));
        clearAllFields();

    } else
    {
        _mess_label->setText(tr("Le chemin n’existe pas !"));
        _mess_label->show();
    }
}
