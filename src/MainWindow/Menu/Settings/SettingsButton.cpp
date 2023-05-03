//
// Created by Rahman on 19/04/2023.
//

#include "SettingsButton.hpp"

SettingsButton::SettingsButton(const Type &type, QWidget *parent) : QGroupBox(parent)
{
    m_layout = new QHBoxLayout(this);
    m_label = new QLabel(this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    switch (type)
    {
        case Type::General:
            m_label->setText(tr("General"));
            m_frame = new GeneralFrame;
            break;
        case Type::Rclone:
            m_label->setText(tr("Rclone"));
            m_frame = new RcloneFrame;
            break;
        case Type::Appearance:
            m_label->setText(tr("Apparence"));
            m_frame = new AppearanceFrame;
            break;
    }

    m_label->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_label);
}

void SettingsButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}
