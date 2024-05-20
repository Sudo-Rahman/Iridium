#include "CircularProgressBar.hpp"
#include <QPainter>
#include <QStyle>
#include <QApplication>

CircularProgressBar::CircularProgressBar(QWidget *parent)
	: QWidget(parent), m_progress(0), m_min(0), m_max(0), m_state(Progress)
{
	m_timer = new QTimer(this);
	m_timer->setInterval(10);
	connect(m_timer, &QTimer::timeout, this, [this]()
	{
		m_timer_counter += 15;
		update();
	});
	m_timer->start();

	m_accentColor = QApplication::palette().highlight().color();
	m_progressColor = QColor(69, 164, 235);
	m_successColor = QColor(128, 211, 132);
	m_errorColor = QColor(255, 118, 118);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

int CircularProgressBar::progress() const { return m_progress; }

void CircularProgressBar::setProgress(double_t value)
{
	if (m_progress != value and value >= m_min and value <= m_max)
	{
		m_progress = value;
		if (m_max != 0 && m_progress >= m_max) { setState(Success); }
		emit progressChanged();
		update(); // Trigger a repaint
	}
}

int CircularProgressBar::minimum() const { return m_min; }

int CircularProgressBar::maximum() const { return m_max; }

void CircularProgressBar::setRange(double_t min, double_t max)
{
	m_min = min;
	m_max = max;
	updateAnimation();
	update();
}

CircularProgressBar::State CircularProgressBar::state() const { return m_state; }

void CircularProgressBar::setState(CircularProgressBar::State state)
{
	if (m_state != state)
	{
		m_state = state;
		update();
	}
}

void CircularProgressBar::setSize(const int &size)
{
	auto copy = size;
	if (copy < 15)
	{
		qWarning() << "CircularProgressBar::setSize() : size must be greater than 15";
		copy = 15;
	}
	setFixedSize(copy, copy);
}

void CircularProgressBar::showPercentText(bool show) { m_show_percent = show; }

void CircularProgressBar::updateAnimation() const
{
	if (m_min == 0 && m_max == 0)
	{
		// Infinite mode
		m_timer->start();
	}
	else
	{
		// Finite mode
		m_timer->stop();
	}
}

QColor CircularProgressBar::currentColor() const
{
	switch (m_state)
	{
		case Success:
			return m_successColor;
		case Error:
			return m_errorColor;
		case Progress:
		default:
			return m_progressColor;
	}
}

void CircularProgressBar::setBaseSize(const QSize &) {}
void CircularProgressBar::setBaseSize(int basew, int baseh) {}
void CircularProgressBar::setFixedSize(const QSize &) {}
void CircularProgressBar::setFixedSize(int w, int h) { QWidget::setFixedSize(w, h); }
void CircularProgressBar::setFixedWidth(int w) {}
void CircularProgressBar::setFixedHeight(int h) {}

void CircularProgressBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	if (size().height() < 50)
		pen.setWidth(int(height() * 0.25));
	else
		pen.setWidth(int(height() * 0.15));
	pen.setCapStyle(Qt::RoundCap);

	int width = size().width();
	int height = size().height();
	int side = qMin(width, height);

	painter.setViewport((width - side) / 2, (height - side) / 2, side, side);

	auto marginRect = rect().marginsRemoved(
		QMargins(15 * side / 100, 15 * side / 100, 15 * side / 100, 15 * side / 100));

	// Draw the background circle
	pen.setColor(palette().color(QPalette::Mid));
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	painter.drawEllipse(marginRect);

	// Draw the progress arc
	pen.setColor(currentColor());
	painter.setPen(pen);

	if (m_min == 0 && m_max == 0)
	{
		// Infinite mode
		// painter.drawArc(marginRect, 90 * 16, -int(360 * 16 * (m_progress / 100.0)));
		painter.drawArc(marginRect, -int(90 * 16 + m_timer_counter),
		                int(-(360 * 16 * 0.3)));
	}
	else
	{
		// Finite mode
		int spanAngle = int((m_progress - m_min) * 360.0 / (m_max - m_min) * 16);
		painter.drawArc(marginRect, 90 * 16, -spanAngle);
	}
	// Draw the progress percentage if the widget size is greater than 100x100 and not in infinite mode
	if (side >= 100 and (m_min != 0 or m_max != 0) and m_show_percent)
	{
		QString percentageText = QString::number(static_cast<int>(static_cast<float>(m_progress) / m_max * 100)) + "%";
		QFont font = painter.font();
		int fontSize = side / 6; // Dynamically adjust font size
		font.setPointSize(fontSize);
		font.setBold(true);
		painter.setFont(font);
		painter.setPen(palette().color(QPalette::WindowText));

		painter.drawText(rect(), Qt::AlignCenter, percentageText);
	}
}
