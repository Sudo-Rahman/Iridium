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
			m_timerCounter += 0.01;
		else
			m_timerCounter += QWidget::width() * 0.005;
		if (m_type == Linear and m_timerCounter > QWidget::width() * 1.2)
			m_timerCounter = 0;

		update();
	});
	setContentsMargins(0, 0, 0, 0);
//	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

void ProgressBar::setValue(double_t mValue)
{
	m_value = mValue;
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
}

void ProgressBar::setType(ProgressBar::Type type)
{
	m_type = type;
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
}

/**
 * @brief draw circular progress bar with m_value and m_maxValue
 * @param painter
 */
void ProgressBar::drawCircular(QPainter &painter)
{

	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setWidth(int(QWidget::height() * 0.15));
	pen.setCapStyle(Qt::RoundCap);
	pen.setColor(QWidget::palette().color(QPalette::Light));
	// draw line circle
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	painter.drawEllipse(rect().marginsRemoved(QMargins(10, 10, 10, 10)));


	if (m_isIndeterminate)
	{
		// draw indeterminate progress circle
		pen.setColor(QColor(QWidget::palette().color(QPalette::Highlight).red(),
							QWidget::palette().color(QPalette::Highlight).green()+10 ,
							QWidget::palette().color(QPalette::Highlight).blue()+50, 255));
		painter.setPen(pen);
		// start at 180° and rotate 360°
		painter.drawArc(rect().marginsRemoved(QMargins(10, 10, 10, 10)), int(-(90 * 16 * m_timerCounter)),
						-(360 * 16 * ((int) m_timerCounter + 25)) / 100);
	} else
	{
		// draw progress circle
		if (m_value == m_maxValue)
			pen.setColor(QColor(QWidget::palette().color(QPalette::Highlight).red(),
								QWidget::palette().color(QPalette::Highlight).green() + 50,
								QWidget::palette().color(QPalette::Highlight).blue(), 255));
		else
			pen.setColor(QWidget::palette().color(QPalette::Highlight));

		painter.setPen(pen);
		painter.drawArc(rect().marginsRemoved(QMargins(10, 10, 10, 10)), 90 * 16,
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
	pen.setWidth(QWidget::height() * 0.5);
	pen.setCapStyle(Qt::RoundCap);
	pen.setColor(QWidget::palette().color(QPalette::Light));


	// draw line
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	// draw line vertical centered
	auto y = rect().marginsRemoved(QMargins(10, 10, 10, 10)).center().y();
	auto start = QPoint(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft().x(), y);
	auto end = QPoint(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x(), y);
	painter.drawLine(start, end);
	pen.setWidth(QWidget::height() * 0.4);

	if (m_isIndeterminate)
	{
		// draw indeterminate progress line
		pen.setColor(QWidget::palette().color(QPalette::Highlight));
		painter.setPen(pen);

		// start in front of progress line
		start += QPoint(m_timerCounter - (QWidget::width() * 0.25), 0);

		if (start.x() < rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft().x())
			start.setX(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft().x());

		if (start.x() > rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x())
			start.setX(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x());


		end = QPoint(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft().x(), y) +
			  QPoint(m_timerCounter, 0);
		if (end.x() > rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x())
			end.setX(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x());

		painter.drawLine(start, end);

	} else
	{
		// draw progress line
		if (m_value == m_maxValue)
			pen.setColor(QColor(QWidget::palette().color(QPalette::Highlight).red(),
								QWidget::palette().color(QPalette::Highlight).green() + 50,
								QWidget::palette().color(QPalette::Highlight).blue(), 255));
		else
			pen.setColor(QWidget::palette().color(QPalette::Highlight));
		painter.setPen(pen);
		end = QPoint(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft().x(), y) +
			  QPoint((m_value * QWidget::width()) / m_maxValue, 0);
		if (end.x() > rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x())
			end.setX(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topRight().x());
		if (m_value > m_minValue)
			painter.drawLine(rect().marginsRemoved(QMargins(10, 10, 10, 10)).topLeft(),
							 end);

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
}

void ProgressBar::setRange(double_t min, double_t max)
{
	if (max == min and max == 0)
		setIsIndeterminate(true);
	setMaxValue(max);
	setMinValue(min);
}
