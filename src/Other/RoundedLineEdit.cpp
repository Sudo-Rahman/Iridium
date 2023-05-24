//
// Created by Rahman on 08/04/2023.
//

#include "RoundedLineEdit.hpp"
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QFontMetrics>

RoundedLineEdit::RoundedLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setClearButtonEnabled(true);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setAttribute(Qt::WA_MacSmallSize, true);
    _boder_radius = QString::number(QWidget::height() / 2.5);
    normalBorder();
    setTextMargins(5, 3, 5, 3);
}

void RoundedLineEdit::addBlur()
{
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(20);
    effect->setOffset(0, 0);
    effect->setColor(palette().color(QPalette::Highlight));
    this->setGraphicsEffect(effect);
}

bool RoundedLineEdit::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::FocusIn:
            addBlur();
            break;
        case QEvent::FocusOut:
            setGraphicsEffect(nullptr);
            break;
        case QEvent::PaletteChange:
            setStyleSheet(_styleSheet);
            break;
        case QEvent::Resize:
            break;
        default:
            break;
    }
    return QLineEdit::event(event);
}

void RoundedLineEdit::reset()
{
    clear();
    normalBorder();
}

void RoundedLineEdit::redBorder()
{
    _styleSheet = "QLineEdit { border: 1px solid red; border-radius: " + _boder_radius + "; }";
    setStyleSheet(_styleSheet);
}

void RoundedLineEdit::normalBorder()
{
    _styleSheet = "QLineEdit { border: 1px solid palette(highlight); border-radius: " + _boder_radius + "; }";
    setStyleSheet(_styleSheet);
}

void RoundedLineEdit::setStyleSheet(const QString &styleSheet)
{
    QLineEdit::setStyleSheet(styleSheet);
}