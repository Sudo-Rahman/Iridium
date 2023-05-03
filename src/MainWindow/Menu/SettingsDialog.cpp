//
// Created by Rahman on 16/04/2023.
//

#include <QLabel>
#include "SettingsDialog.hpp"
#include <AppearanceFrame.hpp>
#include <RcloneFrame.hpp>
#include <SettingsButton.hpp>

SettingsDialog::SettingsDialog(QWidget *parent)
{
    setWindowTitle(tr("Paramètres"));

    setMinimumSize(700, 500);

    m_left_scrollArea = new QScrollArea(this);
    m_left_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_left_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_left_scrollArea->setStyleSheet("QScrollArea{border:none;}");
    m_left_scrollArea->setFixedWidth(150);

    m_left_scrollAreaWidget = new QWidget(m_left_scrollArea);
    m_left_scrollAreaLayout = new QVBoxLayout(m_left_scrollAreaWidget);
    m_left_scrollAreaLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_left_scrollArea->setWidget(m_left_scrollAreaWidget);
    m_left_scrollArea->setWidgetResizable(true);

    // make not hide widget with resize horizontal
    m_left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::General, this));
    m_left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::Rclone, this));
    m_left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::Appearance, this));

    m_right_scrollArea = new QScrollArea(this);
    m_right_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_right_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_right_scrollArea->setMinimumWidth(500);
    m_right_scrollArea->setWidgetResizable(true);
    // not resize with parent
    m_right_scrollArea->setStyleSheet("QScrollArea{border:none;}");


    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->addWidget(m_left_scrollArea);
    layout->setSpacing(0);
    auto *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    layout->addWidget(line);
    layout->addWidget(m_right_scrollArea);

    m_right_scrollArea->setWidget(m_left_scrollAreaWidget->findChildren<SettingsButton *>().first()->getFrame());


    for (const auto &menu: m_left_scrollAreaWidget->findChildren<SettingsButton *>())
    {
        connect(menu, &SettingsButton::clicked, [menu, this]()
        {
            m_right_scrollArea->takeWidget();
            m_right_scrollArea->setWidget(menu->getFrame());
        });
    }
}
