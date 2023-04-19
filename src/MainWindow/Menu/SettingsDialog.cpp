//
// Created by sr-71 on 16/04/2023.
//

#include <QLabel>
#include "SettingsDialog.hpp"
#include <FolderColorGroupBox.hpp>

SettingsDialog::SettingsDialog(QWidget *parent)
{
	setWindowTitle(tr("Paramètres"));

	setMinimumSize(300,500);

	m_leftScrollArea = new QScrollArea(this);
	m_leftScrollArea->setWidgetResizable(true);
	m_leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_leftScrollArea->setStyleSheet("QScrollArea{border:none;}");

	m_leftScrollAreaWidget = new QWidget(m_leftScrollArea);
	m_leftScrollAreaLayout = new QVBoxLayout(m_leftScrollAreaWidget);
	m_leftScrollAreaLayout->setAlignment(Qt::AlignTop);
	m_leftScrollArea->setWidget(m_leftScrollAreaWidget);

	// make not hide widget with resize horizontal
	m_leftScrollAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	m_leftScrollAreaLayout->addWidget(new QLabel("General"));
	m_leftScrollAreaLayout->addWidget(new QLabel("Appearance"));

	m_rightScrollArea = new QScrollArea(this);
	m_rightScrollArea->setWidgetResizable(true);
	m_rightScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_rightScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_rightScrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	m_rightScrollArea->setStyleSheet("QScrollArea{border:none;}");

	m_rightScrollAreaWidget = new QWidget(m_rightScrollArea);
	m_rightScrollAreaLayout = new QVBoxLayout(m_rightScrollAreaWidget);
	m_rightScrollAreaLayout->setAlignment(Qt::AlignTop);
	m_rightScrollArea->setWidget(m_rightScrollAreaWidget);

	auto *layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 0, 5, 0);
	layout->addWidget(m_leftScrollArea);
	auto *line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	layout->addWidget(line);
	layout->addWidget(m_rightScrollArea);

	m_rightScrollAreaLayout->addWidget(new FolderColorGroupBox(this));
}
