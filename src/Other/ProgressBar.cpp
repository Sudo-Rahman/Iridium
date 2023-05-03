//
// Created by Rahman on 16/04/2023.
//

#include "ProgressBar.hpp"
#include <QPainter>


ProgressBar::ProgressBar(const ProgressBar::Type &type, QWidget *parent) : QWidget(parent)
{
    m_type = type;
    m_max_value = 100;
    m_min_value = 0;
    m_value = 0;
    m_timer = new QTimer(this);
    m_timer->setInterval(5);
    connect(m_timer, &QTimer::timeout, this, [this]()
    {
        if (m_type == Circular)
            m_timer_counter += 10;
        else
            m_timer_counter += QWidget::width() * 0.005;
        if (m_type == Linear and m_timer_counter > QWidget::width() * 1.2)
            m_timer_counter = 0;
        update();
    });
    setContentsMargins(0, 0, 0, 0);
}

void ProgressBar::setValue(double_t mValue)
{
    m_value = mValue;
    if (m_max_value == m_value)
        success();
    else
        update();
}

void ProgressBar::setMaxValue(double_t max)
{
    if (max < 0)
    {
        m_max_value = 0;
        return;
    }
    if (max > m_min_value)
        m_max_value = max;
    update();
}

void ProgressBar::setMinValue(double_t min)
{
    if (min < 0)
    {
        m_min_value = 0;
        return;
    }
    if (min < m_max_value)
        m_min_value = min;
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
        m_timer_counter = 0;
    }
    m_is_indeterminate = isIndeterminate;
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
        pen.setColor(QColor(255, 118, 118));
    else
        pen.setColor(QWidget::palette().color(QPalette::Mid));

    // draw line circle
    auto marginRect = rect().marginsRemoved(QMargins(10, 10, 10, 10));
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(marginRect);


    if (m_state == Progress)
        pen.setColor(QColor(69, 164, 235));
    else if (m_state == Success)
        pen.setColor(QColor(128, 211, 132));

    if (m_is_indeterminate)
    {
        // draw indeterminate progress circle
        painter.setPen(pen);
        // start at 90° and advance 1° every 10 ms start and end
        painter.drawArc(marginRect, -int(90 * 16 + m_timer_counter),
                        int(-(360 * 16 * 0.3)));
    } else
    {
        // draw progress circle
        painter.setPen(pen);
        painter.drawArc(marginRect, 90 * 16,
                        int(-(360 * 16 * m_value) / m_max_value));

        if (not m_show_progress)
            return;
        // draw text percentage
        painter.setPen(QColor(255, 255, 255, 255));
        painter.drawText(rect(), Qt::AlignCenter, QString::number((m_value * 100) / m_max_value) + "%");

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
 * @brief draw linear progress bar with m_value and m_max_value
 * @param painter
 */
void ProgressBar::drawLinear(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(QWidget::height() / 2);
    pen.setCapStyle(Qt::RoundCap);
    if (m_state == Error)
        pen.setColor(QColor(255, 118, 118));
    else
        pen.setColor(QWidget::palette().color(QPalette::Mid));

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
        pen.setColor(QColor(69, 164, 235));
    else if (m_state == Success)
        pen.setColor(QColor(128, 211, 132));

    if (m_is_indeterminate)
    {
        // draw indeterminate progress line
        painter.setPen(pen);

        // start in front of progress line
        start += QPoint(m_timer_counter - (QWidget::width() * 0.25), 0);

        if (start.x() < marginRect.topLeft().x())
            start.setX(marginRect.topLeft().x());

        if (start.x() > marginRect.topRight().x())
            start.setX(marginRect.topRight().x());


        end = QPoint(marginRect.topLeft().x(), y) +
              QPoint(m_timer_counter, 0);
        if (end.x() > marginRect.topRight().x())
            end.setX(marginRect.topRight().x());

        painter.drawLine(start, end);

    } else
    {
        // draw progress line
        painter.setPen(pen);

        end = QPoint(marginRect.topLeft().x(), y) +
              QPoint((m_value * QWidget::width()) / m_max_value, 0);

        if (end.x() > marginRect.topRight().x())
            end.setX(marginRect.topRight().x());

        if (m_value > m_min_value)
            painter.drawLine(QPoint(marginRect.topLeft().x(), y), end);

        if (not m_show_progress)
            return;

        painter.setPen(QWidget::palette().color(QPalette::Text));
        auto text = QString::number((m_value * 100) / m_max_value) + "%";
        QRectF textRect = painter.boundingRect(rect(), Qt::AlignCenter, text);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}

void ProgressBar::setShowProgress(bool showProgress)
{
    m_show_progress = showProgress;
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
