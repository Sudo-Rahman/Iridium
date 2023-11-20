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

    QString _styleSheet{}, _boder_radius{};

public:
    explicit RoundedLineEdit(QWidget *parent = nullptr);

    void reset();

    void redBorder();

    void normalBorder();

protected:

    bool event(QEvent *) override;

private:
    void addBlur();

    void setStyleSheet(const QString &styleSheet);

};


#endif //IRIDIUM_ROUNDEDLINEEDIT_HPP
