//
// Created by sr-71 on 29/03/2023.
//

#include "RemoteWidget.hpp"

#include <utility>
#include <QPainter>
#include <QEvent>
#include <QPropertyAnimation>

RemoteWidget::RemoteWidget(RemoteType type, QString name, QWidget *parent) : QGroupBox(parent), m_type(type),
																			 m_name(std::move(name))
{
	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(10, 10, 10, 10);
	m_layout->setSpacing(15);
	m_layout->setAlignment(Qt::AlignLeft | Qt::AlignCenter);

	auto *labelIcon = new QLabel;
	m_layout->addWidget(labelIcon);
// create pixmap
	QPixmap image = {QString::fromStdString(remoteIco.find(m_type)->second)};
	labelIcon->setPixmap(image.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));


	auto *labelRemoteName = new QLabel(m_name);
	m_layout->addWidget(labelRemoteName);

}

void RemoteWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(QGroupBox::palette().color(QPalette::Midlight));
	m_hover ? painter.setBrush(QGroupBox::palette().color(QPalette::Mid)) : painter.setBrush(
		QGroupBox::palette().color(QPalette::Midlight));

	// draw rounded rect
	QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
	rect.setWidth(rect.width() - 1);
	rect.setHeight(rect.height() - 1);
	painter.drawRoundedRect(rect, 10, 10);

}

bool RemoteWidget::event(QEvent *event)
{
	switch (event->type())
	{
		// mouse hover repaint
		case QEvent::Enter:
			m_hover = true;
			repaint();
			break;
		case QEvent::Leave:
			m_hover = false;
			repaint();
			break;
		case QEvent::MouseButtonPress:
			// change cursor
			setCursor(Qt::PointingHandCursor);
			break;
		case QEvent::MouseButtonRelease:
			// change cursor
			setCursor(Qt::ArrowCursor);
			break;
		default:
			break;
	}
	return QGroupBox::event(event);
}

RemoteType RemoteWidget::getType() const
{
	return m_type;
}

QString RemoteWidget::getName() const
{
	return m_name;
}

