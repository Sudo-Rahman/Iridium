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
    m_language = new QComboBox(this);
    m_language->addItems({"English", "Français"});
    m_language->setItemData(0, QLocale::English, Qt::UserRole);
    m_language->setItemData(1, QLocale::French, Qt::UserRole);
    for (int i = 0; i < m_language->count(); ++i)
    {
        if (m_language->itemData(i, Qt::UserRole).value<QLocale::Language>() ==
            QLocale(Settings::getValue<std::string>(Settings::Language).c_str()).language())
        {
            m_language->setCurrentIndex(i);
            break;
        }
    }
    box1Layout->addRow(tr("Langue : "), m_language);

    layout->addWidget(box1);

    for (const auto &box: findChildren<QGroupBox *>())
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connectSignals();
}


void GeneralFrame::connectSignals()
{
    connect(m_language, &QComboBox::currentIndexChanged, [this](const int &index)
    {
        auto lang = m_language->itemData(index, Qt::UserRole).value<QLocale::Language>();
        Settings::setLanguage(lang);
    });
}