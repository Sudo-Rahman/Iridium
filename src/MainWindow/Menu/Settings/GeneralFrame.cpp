//
// Created by Rahman on 02/05/2023.
//

#include "GeneralFrame.hpp"

#include <QLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <Settings.hpp>

GeneralFrame::GeneralFrame(QWidget *parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(10);

    auto box1 = new QGroupBox(this);
    box1->setTitle(tr("Le changement de langue nécessite un redémarrage de l’application."));
    auto *box1Layout = new QFormLayout(box1);
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
    box1Layout->addRow(tr("Langue : "), _language);

    layout->addWidget(box1);

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
}