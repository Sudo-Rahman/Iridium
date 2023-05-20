//
// Created by Rahman on 19/04/2023.
//

#include <QLabel>
#include <Settings.hpp>
#include <QFormLayout>
#include "FolderColorGroupBox.hpp"


FolderColorGroupBox::FolderColorGroupBox(QWidget *parent) : QGroupBox(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout = new QFormLayout(this);
    _layout->setSpacing(0);
    auto colorLayout = new QHBoxLayout();
    colorLayout->setContentsMargins(50, 0, 0, 0);
    colorLayout->setSpacing(10);
    _layout->addRow(tr("Couleur dossier"), colorLayout);
    _buttons << new FolderColorButton(QColor(69, 164, 235)) <<
             new FolderColorButton(QColor(128, 211, 132)) <<
             new FolderColorButton(QColor(150, 150, 150)) <<
             new FolderColorButton(QColor(255, 176, 92)) <<
             new FolderColorButton(QColor(255, 125, 169)) <<
             new FolderColorButton(QColor(227, 130, 241)) <<
             new FolderColorButton(QColor(255, 118, 118)) <<
             new FolderColorButton(QColor(255, 209, 102));

    // get current color
    _buttons.at(Settings::getValue<int>(Settings::DirIconColor))->check(true);

    for (auto *button: _buttons)
    {
        colorLayout->addWidget(button);
        connect(button, &FolderColorButton::clicked, this, [this, button]()
        {
            for (auto *b: _buttons)
                b->check(b == button);
            // get place in buttons in list
            auto index = _buttons.indexOf(button);
            // translate to Settings::ThemeColor
            auto color = static_cast<Settings::ThemeColor>(index);
            Settings::changeDirIcon(color);
        });
    }
}

FolderColorButton::FolderColorButton(const QColor &color, QWidget *parent) : QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(30, 30);
    _color = color;
    _color_normal = color;
    _color_darker = color.darker();
}

void FolderColorButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
// draw inner circle
    painter.setPen(Qt::NoPen);
    painter.setBrush(_color);
    painter.drawEllipse(2, 2, width() - 4, height() - 4);

// draw inner small white circle if checked
    if (_checked)
    {
        painter.setBrush(Qt::white);
        painter.drawEllipse(10, 10, width() - 20, height() - 20);
    }
}

void FolderColorButton::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
// if pressed on the button color darker it
    if (event->button() == Qt::LeftButton)
    {
        _color = _color_darker;
        update();
    }
}

void FolderColorButton::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    // if released on the button color lighter it
    if (event->button() == Qt::LeftButton)
    {
        _color = _color_normal;
        update();
    }
}