//
// Created by rahman on 13/01/23.
//

#include <QPainter>
#include <QMessageBox>
#include <QIterator>

#include "RemoteConfigParamsFrame.hpp"

using namespace std;

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{
    _rclone = Rclone::create_unique();

    _layout = new QVBoxLayout(this);

    _form_layout = new QFormLayout;
    _form_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    _form_layout->setFormAlignment(Qt::AlignTop);
    _form_layout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(_form_layout);

    _remote_name = new RoundedLineEdit(this);
    _form_layout->addRow(tr("Nom : "), _remote_name);
}

/**
 * @brief creation of the ui
 */
void RemoteConfigParamsFrame::createUi()
{
    auto *tmpwidlay = new QHBoxLayout;

    _login = new QPushButton(tr("Se connecter"), this);
    _login->setDefault(true);
    _login->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    tmpwidlay->addWidget(_login, Qt::AlignTop);
    tmpwidlay->setAlignment(_login, Qt::AlignTop);

    _cancel = new QPushButton(tr("Annuler"), this);
    _cancel->hide();
    _cancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tmpwidlay->addWidget(_cancel, Qt::AlignTop);
    tmpwidlay->setAlignment(_cancel, Qt::AlignTop);

    _layout->addLayout(tmpwidlay);

    connect(_login, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);

    connect(_cancel, &QPushButton::clicked, this, [this]()
    {
        _rclone->kill();
        _cancel->hide();
        _login->show();
    });

    _mess_label = new QLabel(this);
    _mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
    _mess_label->hide();
    _mess_label->setAutoFillBackground(true);
    QPalette p;
    p.setColor(QPalette::WindowText, Qt::red);
    _mess_label->setPalette(p);
    _layout->addWidget(_mess_label, Qt::AlignTop);
    _layout->setAlignment(_mess_label, Qt::AlignTop);

    for (auto &field: findChildren<RoundedLineEdit *>())
    {
        connect(field, &QLineEdit::textChanged, this, [this, field]()
        {
            _mess_label->hide();
            field->normalBorder();
        });
    }

}

/**
 * @brief add a new remote  function called when the user click on the login button
 */
void RemoteConfigParamsFrame::addRemote()
{
    if (_remote_name->text().isEmpty())
    {
        _remote_name->redBorder();
        _mess_label->show();
        _mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
        return;
    }

    connect(_rclone.get(), &Rclone::started, this, [this]()
    {
        _login->hide();
        _cancel->show();
    });
    connect(_rclone.get(), &Rclone::finished, this, [this](int exit)
    {
        if (exit == 0)
        {
            emit remoteAdded();
            QMessageBox::information(this, tr("Succès"),
                                     tr("Le disque %1 a été ajouté avec succès").arg(_remote_name->text()));
            clearAllFields();
        } else
        {
            auto msgBox = QMessageBox();
            msgBox.setWindowTitle(tr("Erreur"));
            msgBox.setText(tr("Une erreur est survenue lors de la configuration du serveur distant !"));
            msgBox.setDetailedText(_rclone->readAllError().back().c_str());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        _cancel->hide();
        _login->show();
    });

    auto rclone_liste_remote = Rclone::create_unique();
    rclone_liste_remote->listRemotes();
    rclone_liste_remote->waitForFinished();
    _lst_remote = rclone_liste_remote->getData();

}

/**
 * @brief check if all fields are filled
 * @return true if all fields are filled
 */
bool RemoteConfigParamsFrame::checkFields()
{
    bool ok = true;
    if (_lst_remote.contains(_remote_name->text().toStdString() + ":"))
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Le nom du disque est déjà utilisé !"));
        return false;
    }
    for (auto &field: findChildren<RoundedLineEdit *>())
    {
        if (field->text().isEmpty())
        {
            field->redBorder();
            _mess_label->show();
            _mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
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
    for (auto lineEdit: findChildren<RoundedLineEdit *>()) { lineEdit->reset(); }
}

/**
 * @brief reset the frame
 */
void RemoteConfigParamsFrame::reset()
{
    clearAllFields();
    _mess_label->hide();
}