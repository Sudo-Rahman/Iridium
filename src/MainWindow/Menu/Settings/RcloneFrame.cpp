//
// Created by Rahman on 19/04/2023.
//

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <Settings.hpp>
#include "RcloneFrame.hpp"

RcloneFrame::RcloneFrame(QWidget *parent) : QFrame(parent)
{
    setMinimumWidth(400);
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    auto group1 = new QGroupBox(this);
    auto group1Layout = new QFormLayout(group1);
    m_parallel_transfers = new QSpinBox(this);
    m_parallel_transfers->setRange(1, 100);
    m_parallel_transfers->setValue(QString(Rclone::getFlag(Rclone::Transfers).value.c_str()).toInt());
    group1Layout->addRow(tr("Transfert simultané : "), m_parallel_transfers);

    auto group2 = new QGroupBox(this);
    auto group2Layout = new QFormLayout(group2);
    m_load_type = new QComboBox(this);
    m_load_type->addItem(tr("Dynamic"));
    m_load_type->addItem(tr("Static"));
    m_load_type->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
    m_max_depth = new QSpinBox(this);
    m_max_depth->setRange(2, 10);
    m_max_depth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
    group2Layout->addRow(tr("Type de chargement : "), m_load_type);
    group2Layout->addRow(tr("Profondeur : "), m_max_depth);
    if (RcloneFileModelDistant::loadType() == RcloneFileModelDistant::Dynamic)
        m_max_depth->setEnabled(false);

    auto group3 = new QGroupBox(this);
    auto group3Layout = new QFormLayout(group3);
    m_max_rclone_execution = new QSpinBox(this);
    m_max_rclone_execution->setRange(1, 100);
    m_max_rclone_execution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
    group3Layout->addRow(tr("Nombre maximum d'éxecution simultané : "), m_max_rclone_execution);
    if (RcloneFileModelDistant::loadType() == RcloneFileModelDistant::Dynamic)
        m_max_rclone_execution->setEnabled(false);


    connect(m_load_type, &QComboBox::currentIndexChanged, [this](int index)
    {
        if (index == RcloneFileModelDistant::Dynamic)
        {
            m_max_depth->setEnabled(false);
            m_max_rclone_execution->setEnabled(false);
        } else
        {
            m_max_depth->setEnabled(true);
            m_max_rclone_execution->setEnabled(true);
        }
    });

    auto group4 = new QGroupBox(this);
    auto group4Layout = new QFormLayout(group4);
    m_stats_refresh = new QSpinBox(this);
    m_stats_refresh->setRange(100, 5000);
    m_stats_refresh->setSingleStep(100);;
    m_stats_refresh->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
    group4Layout->addRow(tr("Durée de rafraichissement des statistiques (ms) : "), m_stats_refresh);

    auto *group5 = new QGroupBox(this);
    auto *group5Layout = new QFormLayout(group5);
    auto reset = new QPushButton(tr("Réinitialiser"), this);
    group5Layout->addWidget(reset);
    connect(reset, &QPushButton::clicked, [this]()
    {
        Settings::resetSettings(Settings::Rclone);
        m_parallel_transfers->setValue(std::stoi(Settings::getRcloneFlag(Rclone::Transfers)));
        m_load_type->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
        m_max_depth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
        m_max_rclone_execution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
        m_stats_refresh->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
    });


    layout->addWidget(group1);
    layout->addWidget(group2);
    layout->addWidget(group3);
    layout->addWidget(group4);
    layout->addWidget(group5);

    for (auto *group: findChildren<QGroupBox *>())
        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connectSignals();
}

void RcloneFrame::connectSignals()
{
    connect(m_max_rclone_execution, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setValue(Settings::MaxProcess, value);
        RcloneManager::setMaxProcess(value);
    });
    connect(m_parallel_transfers, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Transfers, std::to_string(value));
    });
    connect(m_load_type, &QComboBox::currentIndexChanged, [=](int index)
    {
        RcloneFileModelDistant::setLoadType(static_cast<RcloneFileModelDistant::Load>(index));
        Settings::setValue(Settings::LoadType, index);
    });
    connect(m_max_depth, &QSpinBox::valueChanged, [=](int value)
    {
        RcloneFileModelDistant::setMaxDepth(value);
        Settings::setValue(Settings::MaxDepth, value);
    });
    connect(m_stats_refresh, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Stats, std::to_string(value / 1000.0));
    });
}
