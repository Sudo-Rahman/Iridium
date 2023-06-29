//
// Created by Rahman on 02/05/2023.
//

#include "GeneralFrame.hpp"

#include <QLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <Settings.hpp>

GeneralFrame::GeneralFrame(QWidget *parent) : QFrame(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(10);

    auto box = new QGroupBox(this);
    box->setTitle(tr("Le changement de langue nécessite un redémarrage de l’application."));
    auto *boxLayout = new QFormLayout(box);
    _language = new QComboBox(this);
    _language->addItems({"English", "Français"});
    _language->setItemData(0, QLocale::English, Qt::UserRole);
    _language->setItemData(1, QLocale::French, Qt::UserRole);
    for (int i = 0; i < _language->count(); ++i)
    {
        if (_language->itemData(i, Qt::UserRole).value<QLocale::Language>() ==
            QLocale(Settings::getValue<std::string>(Settings::Language).c_str()).language())
        {
            _language->setCurrentIndex(i);
            break;
        }
    }

    boxLayout->addRow(tr("Langue : "), _language);
    layout->addWidget(box);

    box = new QGroupBox(this);
    boxLayout = new QFormLayout(box);
    _reload_time = new QSpinBox(this);
    _reload_time->setRange(1, 60);
    _reload_time->setValue(Settings::getValue<int>(Settings::ReloadTime));

    boxLayout->addRow(tr("Temps de rafraîchissement automatic du dossier courant : "), _reload_time);
    layout->addWidget(box);

    for (const auto &box: findChildren<QGroupBox *>())
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connectSignals();
}


void GeneralFrame::connectSignals()
{
    connect(_language, &QComboBox::currentIndexChanged, [this](const int &index)
    {
        auto lang = _language->itemData(index, Qt::UserRole).value<QLocale::Language>();
        Settings::setLanguage(lang);
    });

    connect(_reload_time, &QSpinBox::valueChanged, [](const int &value)
    {
        Settings::setValue(Settings::ReloadTime, value);
        Iridium::Global::reload_time = value;
    });
}