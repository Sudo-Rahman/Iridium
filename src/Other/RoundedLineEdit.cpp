//
// Created by sr-71 on 08/04/2023.
//

#include "RoundedLineEdit.hpp"
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QFontMetrics>

RoundedLineEdit::RoundedLineEdit(QWidget *parent) : QLineEdit(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setMinimumHeight(40);
	setAttribute(Qt::WA_MacShowFocusRect, false);
	m_timer = new QTimer(this);
	m_timer->setInterval(600);
	connect(m_timer, &QTimer::timeout, this, [this]()
	{
		m_addCusrsor = !m_addCusrsor;
		update();
	});

	// wait timer if text written
	connect(this, &QLineEdit::textChanged, this, [this](const QString &text)
	{
		m_addCusrsor = true;
		m_timer->stop();
		m_timer->start();
		update();
	});
}

void RoundedLineEdit::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QLineEdit::palette().color(QPalette::Dark));
	painter.setBrush(QLineEdit::palette().color(QPalette::Light));
	painter.drawRoundedRect(QLineEdit::rect().marginsRemoved(QMargins(5, 5, 5, 5)), 10, 10);

	// draw text
	painter.setPen(QLineEdit::palette().color(QPalette::Text));
	painter.drawText(QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)), QLineEdit::alignment(),
					 QLineEdit::text());

	// draw placeholder text
	if (QLineEdit::text().isEmpty())
	{
		painter.setPen(QLineEdit::palette().color(QPalette::PlaceholderText));
		painter.drawText(QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)), QLineEdit::alignment(),
						 QLineEdit::placeholderText());
	}

	// draw cursor if focused
	if (QLineEdit::hasFocus() and m_addCusrsor)
	{
		painter.setPen(QLineEdit::palette().color(QPalette::Text));
		QFontMetrics fm(QLineEdit::font());
		int textWidth = fm.horizontalAdvance(QLineEdit::text());

		// draw line
		painter.drawLine(QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)).x() + textWidth +2,
						 QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)).y() +2,
						 QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)).x() + textWidth +2,
						 QLineEdit::rect().marginsRemoved(QMargins(10, 10, 10, 10)).y() + 18);
	}

//	QLineEdit::paintEvent(event);
}

void RoundedLineEdit::addBlur()
{
	auto effect = new QGraphicsDropShadowEffect(this);
	effect->setBlurRadius(20);
	effect->setOffset(0, 0);
	effect->setColor(QLineEdit::palette().color(QPalette::Dark));
	this->setGraphicsEffect(effect);
}

bool RoundedLineEdit::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::FocusIn:
			addBlur();
			m_timer->start();
			break;
		case QEvent::FocusOut:
			setGraphicsEffect(nullptr);
			m_timer->stop();
			break;
		default:
			break;
	}
	return QLineEdit::event(event);
}
