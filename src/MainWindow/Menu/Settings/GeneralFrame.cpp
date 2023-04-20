//
// Created by sr-71 on 19/04/2023.
//

#include <QVBoxLayout>
#include <QGroupBox>
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
	auto groupe1Layout = new QHBoxLayout(group1);
	m_maxRcloneExecution = new QSpinBox(this);
	m_maxRcloneExecution->setRange(1, 100);
	m_maxRcloneExecution->setValue(Settings::getValue<uint8_t>(Settings::MaxProcess));
	groupe1Layout->addWidget(new QLabel(tr("Max Rclone Execution : "), this));
	groupe1Layout->addWidget(m_maxRcloneExecution);


	auto group2 = new QGroupBox(this);
	auto groupe2Layout = new QHBoxLayout(group2);
	m_parallelTransfers = new QSpinBox(this);
	m_parallelTransfers->setRange(1, 100);
	m_parallelTransfers->setValue(QString(Rclone::getFlag("transfers").c_str()).toInt());
	groupe2Layout->addWidget(new QLabel(tr("Transfers simultané : "), this));
	groupe2Layout->addWidget(m_parallelTransfers);

	auto group3 = new QGroupBox(this);
	auto groupe3Layout = new QHBoxLayout(group3);
	m_loadType = new QComboBox(this);
	m_loadType->addItem(tr("Dynamic"));
	m_loadType->addItem(tr("Static"));
	m_loadType->setCurrentIndex(Settings::getValue<uint8_t>(Settings::LoadType));
	m_maxDepth = new QSpinBox(this);
	m_maxDepth->setRange(1, 100);
	m_maxDepth->setValue(Settings::getValue<uint8_t>(Settings::MaxDepth));
	groupe3Layout->addWidget(new QLabel(tr("Type de chargement : "), this));
	groupe3Layout->addWidget(m_loadType);
	groupe3Layout->addWidget(m_maxDepth);
	if (RcloneFileModelDistant::loadType() == RcloneFileModelDistant::Dynamic)
		m_maxDepth->setEnabled(false);
	connect(m_loadType, &QComboBox::currentIndexChanged, [=](int index)
	{
		if (index == RcloneFileModelDistant::Dynamic)
			m_maxDepth->setEnabled(false);
		else
			m_maxDepth->setEnabled(true);
	});

	layout->addWidget(group1);
	layout->addWidget(group2);
	layout->addWidget(group3);

	for (auto *group: findChildren<QGroupBox *>())
	{
		group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	}
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
		Rclone::setFlag("transfers", std::to_string(value));
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
}
