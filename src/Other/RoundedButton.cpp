//
// Created by Rahman on 08/04/2023.
//

#include "RoundedButton.hpp"
#include <QEvent>
#include <QPainter>
#include <QGraphicsDropShadowEffect>


/**
 * @brief Construct a new Tree File View Rounded Button:: Tree File View Rounded Button object
 * @param text
 * @param parent
 */
RoundedButton::RoundedButton(const QString &text, QWidget *parent) : QPushButton(text, parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // size font to fit button
    QFont font = this->font();
    font.setPixelSize(17);
    setFont(font);
}

void RoundedButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPushButton::palette().color(QPalette::Light));
    _click ? painter.setBrush(QPushButton::palette().color(QPalette::Window)) : painter.setBrush(
            QPushButton::palette().color(QPalette::Mid).lighter(130));

    // draw rounded rect
    QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
    rect.setWidth(rect.width());
    rect.setHeight(rect.height());
    painter.drawRoundedRect(rect, 10, 10);

    // draw text
    painter.setPen(QPushButton::palette().color(QPalette::WindowText));
    painter.drawText(rect, Qt::AlignCenter, text());
}

bool RoundedButton::event(QEvent *event)
{

    switch (event->type())
    {
        // mouse hover repaint
        case QEvent::Enter:
            _hover = true;
            addBlur();
            break;
        case QEvent::Leave:
            _hover = false;
            addBlur();
            break;
        case QEvent::MouseButtonPress:
            // change cursor
            _click = true;
            repaint();
            setCursor(Qt::PointingHandCursor);
            break;
        case QEvent::MouseButtonRelease:
            // change cursor
            _click = false;
            repaint();
            setCursor(Qt::ArrowCursor);
            break;
        default:
            break;
    }
    return QPushButton::event(event);
}

void RoundedButton::addBlur()
{
    // if not hover remove effect
    if (!_hover)
    {
        this->setGraphicsEffect(nullptr);
        return;
    }
    // resize effect
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(15);
    effect->setOffset(0, 0);
    effect->setColor(QPushButton::palette().color(QPalette::Dark));
    this->setGraphicsEffect(effect);
}
