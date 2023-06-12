//
// Created by Rahman on 19/04/2023.
//

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <Settings.hpp>
#include <QFileDialog>
#include "RcloneFrame.hpp"

RcloneFrame::RcloneFrame(QWidget *parent) : QFrame(parent)
{
    setMinimumWidth(400);
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    auto group = new QGroupBox(this);
    auto gridLayout = new QGridLayout(group);
    _rclone_path = new QLineEdit(this);
    _rclone_path->setText(Settings::getValue<std::string>(Settings::Node::RclonePath).c_str());
    _rclone_path->setReadOnly(true);
    _rclone_path->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *button = new QPushButton(tr("Changer"), this);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(button, &QPushButton::clicked, this, [this]()
    {
        auto path = QFileDialog::getOpenFileName(this, tr("Chemin vers rclone"), QDir::homePath(), "rclone");
        if (!path.isEmpty()) _rclone_path->setText(path);
    });
    connect(_rclone_path, &QLineEdit::textChanged, this, [](const QString &path)
    {
        Settings::setValue(Settings::Node::RclonePath, path.toStdString());
        Iridium::Global::path_rclone = path.toStdString();
        auto rclone = Rclone::create_unique();
        if (not rclone->version().empty())
        {
            Settings::refreshRemotesList();
            Rclone::check_rclone(true);
        }
    });
    gridLayout->addWidget(new QLabel(tr("Chemin vers rclone : "), this), 0, 0, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(_rclone_path, 1, 0, 1, 1);
    gridLayout->addWidget(button, 1, 1, 1, 1);
    layout->addWidget(group);

    group = new QGroupBox(this);
    auto groupLayout = new QFormLayout(group);
    _parallel_transfers = new QSpinBox(this);
    _parallel_transfers->setRange(1, 100);
    _parallel_transfers->setValue(QString(Rclone::getFlag(Rclone::Transfers).value.c_str()).toInt());
    groupLayout->addRow(tr("Transfert simultané : "), _parallel_transfers);

    layout->addWidget(group);

    auto group2 = new QGroupBox(this);
    auto group2Layout = new QFormLayout(group2);
    _load_type = new QComboBox(this);
    _load_type->addItem(tr("Dynamic"));
    _load_type->addItem(tr("Static"));
    _load_type->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
    _max_depth = new QSpinBox(this);
    _max_depth->setRange(2, 10);
    _max_depth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
    group2Layout->addRow(tr("Type de chargement : "), _load_type);
    group2Layout->addRow(tr("Profondeur : "), _max_depth);
    if (Iridium::Global::load_type == Iridium::Load::Dynamic)
        _max_depth->setEnabled(false);

    auto group3 = new QGroupBox(this);
    auto group3Layout = new QFormLayout(group3);
    _max_rclone_execution = new QSpinBox(this);
    _max_rclone_execution->setRange(1, 30);
    _max_rclone_execution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
    group3Layout->addRow(tr("Nombre maximum d'éxecution simultané : "), _max_rclone_execution);
    if (Iridium::Global::load_type == Iridium::Load::Dynamic)
        _max_rclone_execution->setEnabled(false);


    connect(_load_type, &QComboBox::currentIndexChanged, [this](int index)
    {
        if (index == Iridium::Load::Dynamic)
        {
            _max_depth->setEnabled(false);
            _max_rclone_execution->setEnabled(false);
        } else
        {
            _max_depth->setEnabled(true);
            _max_rclone_execution->setEnabled(true);
        }
    });

    auto group4 = new QGroupBox(this);
    auto group4Layout = new QFormLayout(group4);
    _stats_refresh = new QSpinBox(this);
    _stats_refresh->setRange(100, 5000);
    _stats_refresh->setSingleStep(100);;
    _stats_refresh->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
    group4Layout->addRow(tr("Durée de rafraichissement des statistiques (ms) : "), _stats_refresh);

    auto reset = new QPushButton(tr("Réinitialiser"), this);
    reset->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(reset, &QPushButton::clicked, [this]()
    {
        Settings::resetSettings(Settings::Rclone);
        _parallel_transfers->setValue(std::stoi(Settings::getRcloneFlag(Rclone::Transfers)));
        _load_type->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
        _max_depth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
        _max_rclone_execution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
        _stats_refresh->setValue((int) (std::stod(Settings::getRcloneFlag(Rclone::Flag::Stats)) * 1000));
        _rclone_path->setText(Settings::getValue<std::string>(Settings::Node::RclonePath).c_str());
    });


    layout->addWidget(group2);
    layout->addWidget(group3);
    layout->addWidget(group4);
    layout->addWidget(reset, 0, Qt::AlignHCenter);

    for (auto *group: findChildren<QGroupBox *>())
        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connectSignals();
}

void RcloneFrame::connectSignals()
{
    connect(_max_rclone_execution, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setValue(Settings::MaxProcess, value);
        RcloneManager::setMaxProcess(value);
    });
    connect(_parallel_transfers, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Transfers, std::to_string(value));
    });
    connect(_load_type, &QComboBox::currentIndexChanged, [=](int index)
    {
        RcloneFileModelDistant::setLoadType(static_cast<Iridium::Load>(index));
        Settings::setValue(Settings::LoadType, index);
    });
    connect(_max_depth, &QSpinBox::valueChanged, [=](int value)
    {
        RcloneFileModelDistant::setMaxDepth(value);
        Settings::setValue(Settings::MaxDepth, value);
    });
    connect(_stats_refresh, &QSpinBox::valueChanged, [=](int value)
    {
        Settings::setRcloneFlag(Rclone::Flag::Stats, std::to_string(value / 1000.0));
    });
}
