//
// Created by sr-71 on 16/04/2023.
//

#include <QLabel>
#include "SettingsDialog.hpp"
#include <AppearanceFrame.hpp>
#include <GeneralFrame.hpp>
#include <SettingsButton.hpp>

SettingsDialog::SettingsDialog(QWidget *parent)
{
	setWindowTitle(tr("Paramètres"));

	setMinimumSize(700,500);

	m_leftScrollArea = new QScrollArea(this);
	m_leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_leftScrollArea->setStyleSheet("QScrollArea{border:none;}");
	m_leftScrollArea->setFixedWidth(150);

	m_leftScrollAreaWidget = new QWidget(m_leftScrollArea);
	m_leftScrollAreaLayout = new QVBoxLayout(m_leftScrollAreaWidget);
	m_leftScrollAreaLayout->setAlignment(Qt::AlignTop);
	m_leftScrollArea->setWidget(m_leftScrollAreaWidget);
	m_leftScrollArea->setWidgetResizable(true);

	// make not hide widget with resize horizontal
	m_leftScrollAreaLayout->addWidget(new SettingsButton("General"));
	m_leftScrollAreaLayout->addWidget(new SettingsButton("General"));

	m_rightScrollArea = new QScrollArea(this);
	m_rightScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_rightScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_rightScrollArea->setMinimumWidth(500);
	m_rightScrollArea->setWidgetResizable(true);
	// not resize with parent
	m_rightScrollArea->setStyleSheet("QScrollArea{border:none;}");


	auto *layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 0, 5, 0);
	layout->addWidget(m_leftScrollArea);
	auto *line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	layout->addWidget(line);
	layout->addWidget(m_rightScrollArea);

	auto *general = new GeneralFrame(this);
	m_rightScrollArea->setWidget(general);
}
