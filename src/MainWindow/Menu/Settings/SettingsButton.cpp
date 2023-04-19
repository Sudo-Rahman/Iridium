//
// Created by sr-71 on 19/04/2023.
//

#include "SettingsButton.hpp"

SettingsButton::SettingsButton(const QString &text, QWidget *parent) : QGroupBox(parent)
{
//	setMinimumWidth(400);
	m_layout = new QHBoxLayout(this);
	m_label = new QLabel(text,this);
	m_label->setAlignment(Qt::AlignCenter);
	m_layout->addWidget(m_label);

}
