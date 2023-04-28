//
// Created by sr-71 on 19/04/2023.
//

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <Settings.hpp>
#include "GeneralFrame.hpp"

GeneralFrame::GeneralFrame(QWidget *parent) : QFrame(parent)
{
    setMinimumWidth(400);
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    auto group1 = new QGroupBox(this);
    auto group1Layout = new QHBoxLayout(group1);
    m_parallelTransfers = new QSpinBox(this);
    m_parallelTransfers->setRange(1, 100);
    m_parallelTransfers->setValue(QString(Rclone::getFlag(Rclone::Transfers).value.c_str()).toInt());
    group1Layout->addWidget(new QLabel(tr("Transfers simultané : "), this));
    group1Layout->addWidget(m_parallelTransfers);

    auto group2 = new QGroupBox(this);
    auto group2Layout = new QHBoxLayout(group2);
    m_loadType = new QComboBox(this);
    m_loadType->addItem(tr("Dynamic"));
    m_loadType->addItem(tr("Static"));
    m_loadType->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
    m_maxDepth = new QSpinBox(this);
    m_maxDepth->setRange(2, 10);
    m_maxDepth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
    group2Layout->addWidget(new QLabel(tr("Type de chargement : "), this));
    group2Layout->addWidget(m_loadType);
    group2Layout->addWidget(new QLabel(tr("Profondeur : "), this));
    group2Layout->addWidget(m_maxDepth);
    if (RcloneFileModelDistant::loadType() == RcloneFileModelDistant::Dynamic)
        m_maxDepth->setEnabled(false);

    auto group3 = new QGroupBox(this);
    auto group3Layout = new QHBoxLayout(group3);
    m_maxRcloneExecution = new QSpinBox(this);
    m_maxRcloneExecution->setRange(1, 100);
    m_maxRcloneExecution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
    group3Layout->addWidget(new QLabel(tr("Nombre maximum d'éxecution simultané : "), this));
    group3Layout->addWidget(m_maxRcloneExecution);
    if (RcloneFileModelDistant::loadType() == RcloneFileModelDistant::Dynamic)
        m_maxRcloneExecution->setEnabled(false);


    connect(m_loadType, &QComboBox::currentIndexChanged, [this](int index)
    {
        if (index == RcloneFileModelDistant::Dynamic)
        {
            m_maxDepth->setEnabled(false);
            m_maxRcloneExecution->setEnabled(false);
        } else
        {
            m_maxDepth->setEnabled(true);
            m_maxRcloneExecution->setEnabled(true);
        }
    });

    auto group4 = new QGroupBox(this);
    auto group4Layout = new QHBoxLayout(group4);
    m_statsDuration = new QSpinBox(this);
    m_statsDuration->setRange(100, 5000);
    m_statsDuration->setSingleStep(100);;
    m_statsDuration->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
    group4Layout->addWidget(new QLabel(tr("Durée de rafraichissement des statistiques (ms) : "), this));
    group4Layout->addWidget(m_statsDuration);

    auto *group5 = new QGroupBox(this);
    auto *group5Layout = new QHBoxLayout(group5);
    auto reset = new QPushButton(tr("Réinitialiser"), this);
    group5Layout->addWidget(reset);
    connect(reset, &QPushButton::clicked, [this]()
    {
        Settings::resetSettings();
        m_parallelTransfers->setValue(std::stoi(Settings::getRcloneFlag(Rclone::Transfers)));
        m_loadType->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
        m_maxDepth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
        m_maxRcloneExecution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
        m_statsDuration->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
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

void GeneralFrame::connectSignals()
{
    connect(m_maxRcloneExecution, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setValue(Settings::MaxProcess, value);
        RcloneManager::setMaxProcess(value);
    });
    connect(m_parallelTransfers, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Transfers, std::to_string(value));
    });
    connect(m_loadType, &QComboBox::currentIndexChanged, [=](int index)
    {
        RcloneFileModelDistant::setLoadType(static_cast<RcloneFileModelDistant::Load>(index));
        Settings::setValue(Settings::LoadType, index);
    });
    connect(m_maxDepth, &QSpinBox::valueChanged, [=](int value)
    {
        RcloneFileModelDistant::setMaxDepth(value);
        Settings::setValue(Settings::MaxDepth, value);
    });
    connect(m_statsDuration, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Stats, std::to_string(value / 1000.0));
    });
}
