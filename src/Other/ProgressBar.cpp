//
// Created by sr-71 on 16/04/2023.
//

#include "ProgressBar.hpp"
#include <QPainter>


ProgressBar::ProgressBar(const ProgressBar::Type &type, QWidget *parent) : QWidget(parent)
{
	m_type = type;
	m_maxValue = 100;
	m_minValue = 0;
	m_value = 0;
	m_timer = new QTimer(this);
	m_timer->setInterval(5);
	connect(m_timer, &QTimer::timeout, this, [this]()
	{
		if (m_type == Circular)
			m_timerCounter += 10;
		else
			m_timerCounter += QWidget::width() * 0.005;
		if (m_type == Linear and m_timerCounter > QWidget::width() * 1.2)
			m_timerCounter = 0;
		update();
	});
	setContentsMargins(0, 0, 0, 0);
}

void ProgressBar::setValue(double_t mValue)
{
	m_value = mValue;
	if (m_maxValue == m_value)
		success();
	else
		update();
}

void ProgressBar::setMaxValue(double_t max)
{
	if (max < 0)
	{
		m_maxValue = 0;
		return;
	}
	if (max > m_minValue)
		m_maxValue = max;
	update();
}

void ProgressBar::setMinValue(double_t min)
{
	if (min < 0)
	{
		m_minValue = 0;
		return;
	}
	if (min < m_maxValue)
		m_minValue = min;
	update();
}

void ProgressBar::setType(ProgressBar::Type type)
{
	m_type = type;
	update();
}

void ProgressBar::setIsIndeterminate(bool isIndeterminate)
{
	if (isIndeterminate)
		m_timer->start();
	else
	{
		m_timer->stop();
		m_timerCounter = 0;
	}
	m_isIndeterminate = isIndeterminate;
	update();
}

/**
 * @brief draw circular progress bar
 * @param painter
 */
void ProgressBar::drawCircular(QPainter &painter)
{

	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setWidth(int(QWidget::height() * 0.15));
	pen.setCapStyle(Qt::RoundCap);

	if (m_state == Error)
		pen.setColor(QColor(Qt::darkRed));
	else
		pen.setColor(QWidget::palette().color(QPalette::Light));

	// draw line circle
	auto marginRect = rect().marginsRemoved(QMargins(10, 10, 10, 10));
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	painter.drawEllipse(marginRect);


	if (m_state == Progress)
		pen.setColor(QColor(Qt::blue).lighter(100));
	else if (m_state == Success)
		pen.setColor(QColor(Qt::green).lighter(50));

	if (m_isIndeterminate)
	{
		// draw indeterminate progress circle
		painter.setPen(pen);
		// start at 90° and advance 1° every 10 ms start and end
		painter.drawArc(marginRect, -int(90 * 16 + m_timerCounter),
						int(-(360 * 16 * 0.3)));
	} else
	{
		// draw progress circle
		painter.setPen(pen);
		painter.drawArc(marginRect, 90 * 16,
						int(-(360 * 16 * m_value) / m_maxValue));

		if (not m_showProgress)
			return;
		// draw text percentage
		painter.setPen(QColor(255, 255, 255, 255));
		painter.drawText(rect(), Qt::AlignCenter, QString::number((m_value * 100) / m_maxValue) + "%");

	}

}

void ProgressBar::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	switch (m_type)
	{
		case ProgressBar::Circular:
			drawCircular(painter);
			break;
		case ProgressBar::Linear:
			drawLinear(painter);
			break;
	}
	QWidget::paintEvent(event);
}

/**
 * @brief draw linear progress bar with m_value and m_maxValue
 * @param painter
 */
void ProgressBar::drawLinear(QPainter &painter)
{
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setWidth(QWidget::height() / 2);
	pen.setCapStyle(Qt::RoundCap);
	if (m_state == Error)
		pen.setColor(QColor(Qt::darkRed));
	else
		pen.setColor(QWidget::palette().color(QPalette::Light));

	// draw line
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	// draw line vertical centered
	auto y = QWidget::height() / 2;
	auto marginRect = rect().marginsRemoved(QMargins(10, 0, 10, 0));
	auto start = QPoint(marginRect.topLeft().x(), y);
	auto end = QPoint(marginRect.topRight().x(), y);
	painter.drawLine(start, end);
	pen.setWidth(QWidget::height() / 2);

	if (m_state == Progress)
		pen.setColor(QColor(Qt::blue).lighter(100));
	else if (m_state == Success)
		pen.setColor(QColor(Qt::green).lighter(50));

	if (m_isIndeterminate)
	{
		// draw indeterminate progress line
		painter.setPen(pen);

		// start in front of progress line
		start += QPoint(m_timerCounter - (QWidget::width() * 0.25), 0);

		if (start.x() < marginRect.topLeft().x())
			start.setX(marginRect.topLeft().x());

		if (start.x() > marginRect.topRight().x())
			start.setX(marginRect.topRight().x());


		end = QPoint(marginRect.topLeft().x(), y) +
			  QPoint(m_timerCounter, 0);
		if (end.x() > marginRect.topRight().x())
			end.setX(marginRect.topRight().x());

		painter.drawLine(start, end);

	} else
	{
		// draw progress line
		painter.setPen(pen);

		end = QPoint(marginRect.topLeft().x(), y) +
			  QPoint((m_value * QWidget::width()) / m_maxValue, 0);

		if (end.x() > marginRect.topRight().x())
			end.setX(marginRect.topRight().x());

		if (m_value > m_minValue)
			painter.drawLine(QPoint(marginRect.topLeft().x(), y), end);

		if (not m_showProgress)
			return;

		painter.setPen(QWidget::palette().color(QPalette::Text));
		auto text = QString::number((m_value * 100) / m_maxValue) + "%";
		QRectF textRect = painter.boundingRect(rect(), Qt::AlignCenter, text);
		painter.drawText(textRect, Qt::AlignCenter, text);
	}
}

void ProgressBar::setShowProgress(bool showProgress)
{
	m_showProgress = showProgress;
	update();
}

void ProgressBar::setRange(double_t min, double_t max)
{
	if (max == min and max == 0)
		setIsIndeterminate(true);
	setMaxValue(max);
	setMinValue(min);
}

void ProgressBar::error()
{
	setIsIndeterminate(false);
	setShowProgress(true);
	m_state = Error;
	update();
}

void ProgressBar::success()
{
	setIsIndeterminate(false);
	setShowProgress(true);
	m_state = Success;
	update();
}
