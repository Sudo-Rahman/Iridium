//
// Created by Rahman on 16/04/2023.
//

#include "ProgressBar.hpp"
#include <QPainter>


ProgressBar::ProgressBar(const ProgressBar::Type &type, QWidget *parent) : QWidget(parent)
{
    if(type == Circular)
        setMinimumSize(100, 100);
    else
        setMinimumHeight(10);
    _type = type;
    _max_value = 100;
    _min_value = 0;
    _value = 0;
    _current = _progress;
    _timer = new QTimer(this);
    _timer->setInterval(5);
    connect(_timer, &QTimer::timeout, this, [this]()
    {
        if (_type == Circular)
            _timer_counter += 10;
        else
            _timer_counter += QWidget::width() * 0.005;
        if (_type == Linear and _timer_counter > QWidget::width() * 1.2)
            _timer_counter = 0;
        update();
    });
    setContentsMargins(0, 0, 0, 0);
}

void ProgressBar::setValue(double_t mValue)
{
    _value = mValue;
    _state = Progress;
    if (_max_value == _value)
        success();
    else
        update();
}

void ProgressBar::setMaxValue(double_t max)
{
    if (max < 0)
    {
        _max_value = 0;
        return;
    }
    if (max > _min_value)
        _max_value = max;
    update();
}

void ProgressBar::setMinValue(double_t min)
{
    if (min < 0)
    {
        _min_value = 0;
        return;
    }
    if (min < _max_value)
        _min_value = min;
    update();
}

void ProgressBar::setType(ProgressBar::Type type)
{
    _type = type;
    update();
}

void ProgressBar::setIsIndeterminate(bool isIndeterminate)
{
    if (isIndeterminate)
    {
        _timer->start();
        _current = _highlight;
    }
    else
    {
        _timer->stop();
        _timer_counter = 0;
    }
    _is_indeterminate = isIndeterminate;
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

    if (_state == Error)
        pen.setColor(_error);
    else
        pen.setColor(QWidget::palette().color(QPalette::Mid));

    // draw line circle
    auto marginRect = rect().marginsRemoved(QMargins(10, 10, 10, 10));
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(marginRect);

    pen.setColor(_current);

    if (_is_indeterminate)
    {
        // draw indeterminate progress circle
        painter.setPen(pen);
        // start at 90° and advance 1° every 10 ms start and end
        painter.drawArc(marginRect, -int(90 * 16 + _timer_counter),
                        int(-(360 * 16 * 0.3)));
    } else
    {
        // draw progress circle
        painter.setPen(pen);
        painter.drawArc(marginRect, 90 * 16,
                        int(-(360 * 16 * _value) / _max_value));

        if (not _show_progress)
            return;
        // draw text percentage
        painter.setPen(QColor(255, 255, 255, 255));
        painter.drawText(rect(), Qt::AlignCenter, QString::number((_value * 100) / _max_value) + "%");

    }

}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    switch (_type)
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
 * @brief draw linear progress bar with _value and _max_value
 * @param painter
 */
void ProgressBar::drawLinear(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(QWidget::height() / 2);
    pen.setCapStyle(Qt::RoundCap);
    if (_state == Error)
        pen.setColor(_error);
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

    pen.setColor(_current);

    if (_is_indeterminate)
    {
        // draw indeterminate progress line
        painter.setPen(pen);

        // start in front of progress line
        start += QPoint(_timer_counter - (QWidget::width() * 0.25), 0);

        if (start.x() < marginRect.topLeft().x())
            start.setX(marginRect.topLeft().x());

        if (start.x() > marginRect.topRight().x())
            start.setX(marginRect.topRight().x());


        end = QPoint(marginRect.topLeft().x(), y) +
              QPoint(_timer_counter, 0);
        if (end.x() > marginRect.topRight().x())
            end.setX(marginRect.topRight().x());

        painter.drawLine(start, end);

    } else
    {
        // draw progress line
        painter.setPen(pen);

        end = QPoint(marginRect.topLeft().x(), y) +
              QPoint((_value * QWidget::width()) / _max_value, 0);

        if (end.x() > marginRect.topRight().x())
            end.setX(marginRect.topRight().x());

        if (_value > _min_value)
            painter.drawLine(QPoint(marginRect.topLeft().x(), y), end);

        if (not _show_progress)
            return;

        painter.setPen(QWidget::palette().color(QPalette::Text));
        auto text = QString::number((_value * 100) / _max_value) + "%";
        QRectF textRect = painter.boundingRect(rect(), Qt::AlignCenter, text);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}

void ProgressBar::setShowProgress(bool showProgress)
{
    _show_progress = showProgress;
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
    _state = Error;
    _current = _error;
    update();
}

void ProgressBar::success()
{
    setIsIndeterminate(false);
    setShowProgress(true);
    _state = Success;
    _current = _success;
    update();
}
