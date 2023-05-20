//
// Created by Rahman on 08/04/2023.
//

#ifndef IRIDIUM_ROUNDEDLINEEDIT_HPP
#define IRIDIUM_ROUNDEDLINEEDIT_HPP

#include <QLineEdit>
#include <QTimer>

class RoundedLineEdit : public QLineEdit
{
Q_OBJECT

    QTimer *_timer;
    bool _add_cusrsor{};

public:
    explicit RoundedLineEdit(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

    bool event(QEvent *) override;

private:
    void addBlur();

};


#endif //IRIDIUM_ROUNDEDLINEEDIT_HPP
