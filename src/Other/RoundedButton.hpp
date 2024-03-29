//
// Created by Rahman on 08/04/2023.
//

#ifndef IRIDIUM_ROUNDEDBUTTON_HPP
#define IRIDIUM_ROUNDEDBUTTON_HPP

#include <QPushButton>

class RoundedButton : public QPushButton
{
Q_OBJECT

    bool _hover{}, _click{};

    void addBlur();

public:
    explicit RoundedButton(const QString &text, QWidget *parent = nullptr);

    explicit RoundedButton(QWidget *parent = nullptr) {};

protected:
    void paintEvent(QPaintEvent *event) override;


    virtual bool event(QEvent *event) override;

};

#endif //IRIDIUM_ROUNDEDBUTTON_HPP
