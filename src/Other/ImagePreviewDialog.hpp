//
// Created by sr-71 on 18/09/2023.
//

#ifndef IRIDIUM_IMAGEPREVIEWDIALOG_HPP
#define IRIDIUM_IMAGEPREVIEWDIALOG_HPP


#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QPushButton>

class ImagePreviewDialog : public QDialog
{
Q_OBJECT

private:
    QByteArray _data;
    QLabel *_label_image;
    QPushButton *_extend_button, *_shrink_button, *_rotate_button;
    int _rotate_angle{};
    QSize _size{};
    QVBoxLayout *_layout_button{};
    QTimer *_timer{};
    double _ratio{};
    bool _ok{true};

public:
    explicit ImagePreviewDialog(const QByteArray &, QWidget *parent = nullptr);

    void setData(const QByteArray &data);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void resizeImage(QSize size);

    void rotate();

    void moveButtons();

};


#endif //IRIDIUM_IMAGEPREVIEWDIALOG_HPP
