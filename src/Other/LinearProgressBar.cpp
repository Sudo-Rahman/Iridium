#include <LinearProgressBar.hpp>
#include <QPainter>
#include <QStyle>
#include <QPainterPath>
#include <QApplication>
#include <QResizeEvent>

LinearProgressBar::LinearProgressBar(QWidget *parent)
	: QWidget(parent), m_progress(0), m_min(0), m_max(0), m_state(Progress)
{
	m_timer = new QTimer(this);
	m_timer->setInterval(10);
	connect(m_timer, &QTimer::timeout, this, [this]()
	{
		m_timer_counter += 0.01 * width();
		update();
	});
	m_timer->start();

	m_accentColor = QApplication::palette().highlight().color();
	m_progressColor = QColor(0, 119, 190);
	m_successColor = QColor(50, 192, 88);
	m_errorColor = QColor(251, 81, 88);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

int LinearProgressBar::progress() const { return m_progress; }

void LinearProgressBar::setProgress(double_t value)
{
	if (m_progress != value && value >= m_min && value <= m_max)
	{
		m_progress = value;
		if (m_max != 0 && m_progress >= m_max) { setState(Success); }
		emit progressChanged();
		update(); // Trigger a repaint
	}
}

double_t LinearProgressBar::minimum() const { return m_min; }

double_t LinearProgressBar::maximum() const { return m_max; }

void LinearProgressBar::setRange(double_t min, double_t max)
{
	m_min = min;
	m_max = max;
	updateAnimation();
	update();
}

LinearProgressBar::State LinearProgressBar::state() const { return m_state; }

void LinearProgressBar::setState(LinearProgressBar::State state)
{
	if (m_state != state)
	{
		m_state = state;
		update();
	}
}

void LinearProgressBar::showPercentText(bool show) { m_show_percent = show; }
void LinearProgressBar::error() { setState(Error); }

void LinearProgressBar::updateAnimation() const
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

QColor LinearProgressBar::currentColor() const
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

void LinearProgressBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QRect rect = this->rect().marginsRemoved(QMargins(1, 1, 1, 1));
	int radius = rect.height() / 2;
	int progressWidth = static_cast<int>((rect.width() * (m_progress - m_min)) / (m_max - m_min));

	// Draw background
	painter.setBrush(palette().color(QPalette::Mid));
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(rect, radius, radius);

	// Set up clipping region to ensure the progress bar respects the rounded corners
	QPainterPath clipPath;
	clipPath.addRoundedRect(rect, radius, radius);
	painter.setClipPath(clipPath);

	// Draw progress bar
	QRect progressRect(rect.x(), rect.y(), m_state == Error ? rect.width() : progressWidth, rect.height());
	painter.setBrush(currentColor());

	if (m_min == 0 && m_max == 0)
	{
		int segmentWidth = rect.width() * 0.30;
		int maxCounterValue = rect.width() + segmentWidth;

		// Calculate smooth transition using sine wave
		int x = int(
			(rect.width() + segmentWidth) * 0.5 * (
				0.77 + sin(2 * M_PI * (int(m_timer_counter) % maxCounterValue) / maxCounterValue)) - segmentWidth / 2);

		progressRect.setX(x);
		progressRect.setWidth(segmentWidth);
		painter.drawRoundedRect(progressRect, radius, radius);
	}
	else
	{
		// Finite mode
		painter.drawRoundedRect(progressRect, radius, radius);
	}

	// Draw the progress percentage if needed
	if (m_show_percent)
	{
		QString percentageText = QString::number(static_cast<int>(m_progress / m_max * 100)) + "%";
		QFont font = painter.font();
		int fontSize = height() / 2; // Dynamically adjust font size
		font.setPointSize(fontSize);
		font.setBold(true);
		painter.setFont(font);
		painter.setPen(palette().color(QPalette::WindowText));

		painter.drawText(rect, Qt::AlignCenter, percentageText);
	}
}