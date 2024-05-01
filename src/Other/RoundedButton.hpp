//
// Created by Rahman on 08/04/2023.
//

#pragma once

#include <QPushButton>

class RoundedButton : public QPushButton
{
Q_OBJECT

    QString getStyleSheet();
    uint8_t _radius = 4;
    uint8_t _padding = 4;
    bool _circular = false;

public:
    explicit RoundedButton(const QString &text, QWidget *parent = nullptr);

    explicit RoundedButton(const QIcon &icon, QWidget *parent = nullptr);

    void setRadius(uint8_t radius) { _radius = radius;setStyleSheet(getStyleSheet()); }

    void setPadding(uint8_t padding) { _padding = padding;setStyleSheet(getStyleSheet()); }

    void setCircular(bool circular) { _circular = circular;setStyleSheet(getStyleSheet()); }


};
