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

    _left_scrollArea = new QScrollArea(this);
    _left_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _left_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _left_scrollArea->setStyleSheet("QScrollArea{border:none;}");
    _left_scrollArea->setFixedWidth(150);

    _left_scrollAreaWidget = new QWidget(_left_scrollArea);
    _left_scrollAreaLayout = new QVBoxLayout(_left_scrollAreaWidget);
    _left_scrollAreaLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _left_scrollArea->setWidget(_left_scrollAreaWidget);
    _left_scrollArea->setWidgetResizable(true);

    // make not hide widget with resize horizontal
    _left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::General, this));
    _left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::Rclone, this));
    _left_scrollAreaLayout->addWidget(new SettingsButton(SettingsButton::Type::Appearance, this));

    _right_scrollArea = new QScrollArea(this);
    _right_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _right_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _right_scrollArea->setMinimumWidth(500);
    _right_scrollArea->setWidgetResizable(true);
    // not resize with parent
    _right_scrollArea->setStyleSheet("QScrollArea{border:none;}");


    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->addWidget(_left_scrollArea);
    layout->setSpacing(0);
    auto *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    layout->addWidget(line);
    layout->addWidget(_right_scrollArea);

    _right_scrollArea->setWidget(_left_scrollAreaWidget->findChildren<SettingsButton *>().first()->getFrame());


    for (const auto &menu: _left_scrollAreaWidget->findChildren<SettingsButton *>())
    {
        connect(menu, &SettingsButton::clicked, [menu, this]()
        {
            _right_scrollArea->takeWidget();
            _right_scrollArea->setWidget(menu->getFrame());
        });
    }
}
