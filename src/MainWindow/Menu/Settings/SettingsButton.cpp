//
// Created by Rahman on 19/04/2023.
//

#include "SettingsButton.hpp"

SettingsButton::SettingsButton(const Type &type, QWidget *parent) : QGroupBox(parent)
{
    _layout = new QHBoxLayout(this);
    _label = new QLabel(this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    switch (type)
    {
        case Type::General:
            _label->setText(tr("General"));
            _frame = new GeneralFrame;
            break;
        case Type::Rclone:
            _label->setText(tr("Rclone"));
            _frame = new RcloneFrame;
            break;
        case Type::Appearance:
            _label->setText(tr("Apparence"));
            _frame = new AppearanceFrame;
            break;
    }

    _label->setAlignment(Qt::AlignCenter);
    _layout->addWidget(_label);
}

void SettingsButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}
