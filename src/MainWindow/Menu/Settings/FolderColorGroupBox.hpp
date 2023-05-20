//
// Created by Rahman on 19/04/2023.
//

#ifndef IRIDIUM_FOLDERCOLORGROUPBOX_HPP
#define IRIDIUM_FOLDERCOLORGROUPBOX_HPP

#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

class FolderColorButton;

class FolderColorGroupBox : public QGroupBox
{
Q_OBJECT

    QFormLayout *_layout;
    QList<FolderColorButton *> _buttons;

public:

    explicit FolderColorGroupBox(QWidget *parent = nullptr);

};


class FolderColorButton : public QPushButton
{
    QColor _color, _color_darker, _color_normal;
    bool _checked = false;

public:
    explicit FolderColorButton(const QColor &color, QWidget *parent = nullptr);

    void check(bool checked)
    {
        _checked = checked;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif //IRIDIUM_FOLDERCOLORGROUPBOX_HPP
