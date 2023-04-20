//
// Created by sr-71 on 19/04/2023.
//

#include <QLabel>
#include <Settings.hpp>
#include "FolderColorGroupBox.hpp"


FolderColorGroupBox::FolderColorGroupBox(QWidget *parent) : QGroupBox(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_layout = new QHBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->addWidget(new QLabel(tr("Couleur dossier")));
	m_layout->addStretch();
	auto colorLayout = new QHBoxLayout();
	colorLayout->setContentsMargins(50, 0, 0, 0);
	colorLayout->setSpacing(10);
	m_layout->addLayout(colorLayout);
	m_buttons << new FolderColorButton(QColor(69, 164, 235)) <<
			  new FolderColorButton(QColor(128, 211, 132)) <<
			  new FolderColorButton(QColor(150, 150, 150)) <<
			  new FolderColorButton(QColor(255, 176, 92)) <<
			  new FolderColorButton(QColor(255, 125, 169)) <<
			  new FolderColorButton(QColor(227, 130, 241)) <<
			  new FolderColorButton(QColor(255, 118, 118)) <<
			  new FolderColorButton(QColor(255, 209, 102));

	// get current color
	m_buttons.at(Settings::getValue<int>(Settings::DirIconColor))->check(true);

	for (auto *button: m_buttons)
	{
		colorLayout->addWidget(button);
		connect(button, &FolderColorButton::clicked, this, [this, button]()
		{
			for (auto *b: m_buttons)
				b->check(b == button);
			// get place in buttons in list
			auto index = m_buttons.indexOf(button);
			// translate to Settings::ThemeColor
			auto color = static_cast<Settings::ThemeColor>(index);
			Settings::changeDirIcon(color);
		});
	}
}

FolderColorButton::FolderColorButton(const QColor &color, QWidget *parent) : QPushButton(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedSize(30, 30);
	m_color = color;
	m_colorNormal = color;
	m_colorDarker = color.darker();
}

void FolderColorButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
// draw inner circle
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_color);
	painter.drawEllipse(2, 2, width() - 4, height() - 4);

// draw inner small white circle if checked
	if (m_checked)
	{
		painter.setBrush(Qt::white);
		painter.drawEllipse(10, 10, width() - 20, height() - 20);
	}
}

void FolderColorButton::mousePressEvent(QMouseEvent *event)
{
	QPushButton::mousePressEvent(event);
// if pressed on the button color darker it
	if (event->button() == Qt::LeftButton)
	{
		m_color = m_colorDarker;
		update();
	}
}

void FolderColorButton::mouseReleaseEvent(QMouseEvent *event)
{
	QPushButton::mouseReleaseEvent(event);
	// if released on the button color lighter it
	if (event->button() == Qt::LeftButton)
	{
		m_color = m_colorNormal;
		update();
	}
}