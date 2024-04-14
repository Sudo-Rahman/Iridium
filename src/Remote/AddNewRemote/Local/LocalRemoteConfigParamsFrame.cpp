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
    RemoteConfigParamsFrame::createUi();
    _login->setText(tr("Ajouter"));

    _path_button = new QPushButton(tr("Chemin du dossier"), this);

    _path_lineEdit = new RoundedLineEdit(this);
    _path_lineEdit->setPlaceholderText("/user/home");

    _form_layout->addRow(_path_button, _path_lineEdit);

    connect(_path_button, &QPushButton::clicked, this, [this]()
    {
        auto result = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly |
                                                        QFileDialog::DontResolveSymlinks);
        if (!result.isEmpty())
        {
            _path = result;
            _path_lineEdit->setText(result);
        }
    });

    connect(_path_lineEdit, &QLineEdit::textChanged, this, [this](const QString &text)
    {
        _path = text;
        if (QFileInfo::exists(text))
        {
            _path_lineEdit->normalBorder();
            _path_lineEdit->setToolTip("");
        } else
        {
            _path_lineEdit->redBorder();
            _path_lineEdit->setToolTip(tr("Le chemin n’existe pas !"));
        }
    });
}

void LocalRemoteConfigParamsFrame::addRemote()
{
    if (QFileInfo::exists(_path))
    {
        Settings::addLocalRemote(
                {_path.toStdString(), ire::remote::remote_type::none, _remote_name->text().toStdString()});
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
