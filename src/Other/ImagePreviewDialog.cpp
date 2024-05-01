//
// Created by sr-71 on 18/09/2023.
//

#include <QVBoxLayout>
#include <QFile>
#include <QResizeEvent>
#include <QMovie>
#include "ImagePreviewDialog.hpp"

ImagePreviewDialog::ImagePreviewDialog(const QByteArray &data, QWidget *parent) : QDialog(parent)
{
    _data = data;
    _label_image = new QLabel(this);
    _label_image->setAlignment(Qt::AlignCenter);
    _label_image->setContentsMargins(0, 0, 0, 0);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(_label_image);
    layout->setContentsMargins(0, 0, 0, 0);

    _extend_button = new QPushButton(this);
    _extend_button->setFixedSize(35, 35);
    _extend_button->setStyleSheet("background-color: rgba(0,0,0,0);");
    _extend_button->setIcon(QIcon(":/resources/extend.png"));
    connect(_extend_button, &QPushButton::clicked, this,
            [this] { resizeImage(QSize(_size.width() + 100, _size.height() + 100)); });

    _shrink_button = new QPushButton(this);
    _shrink_button->setFixedSize(35, 35);
    _shrink_button->setStyleSheet("background-color: rgba(0,0,0,0);");
    _shrink_button->setIcon(QIcon(":/resources/shrink.png"));
    connect(_shrink_button, &QPushButton::clicked, this,
            [this] { resizeImage(QSize(_size.width() - 100, _size.height() - 100)); });

    _rotate_button = new QPushButton(this);
    _rotate_button->setFixedSize(35, 35);
    _rotate_button->setStyleSheet("background-color: rgba(0,0,0,0);");
    _rotate_button->setIcon(QIcon(":/resources/rotate-left.png"));
    connect(_rotate_button, &QPushButton::clicked, this, &ImagePreviewDialog::rotate);

    // add button above the image
    _layout_button = new QVBoxLayout();
    _layout_button->addWidget(_rotate_button);
    _layout_button->addWidget(_shrink_button);
    _layout_button->addWidget(_extend_button);

    _label_image->stackUnder(_layout_button->widget());

    _timer = new QTimer(this);
    _timer->setSingleShot(true);

    connect(_timer, &QTimer::timeout, this, [this] { resizeImage(rect().size()); });

    setData(_data);


}

void ImagePreviewDialog::setData(const QByteArray &data)
{
    QPixmap pixmap = QPixmap();
    _ok = pixmap.loadFromData(data);
    if (not _ok)
    {
        _label_image->setText(tr("Erreur lors du chargement de l'image"));
        auto palette = _label_image->palette();
        palette.setColor(QPalette::WindowText, Qt::red);
        _label_image->setPalette(palette);
        _label_image->setAlignment(Qt::AlignCenter);
        return;
    }
    _ratio = pixmap.width() / (double) pixmap.height();
    _size = {400, int(400 / _ratio)};
    setMinimumSize(_size);
    _rotate_angle = 0;
    _label_image->setPixmap(pixmap.scaled(_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void ImagePreviewDialog::resizeImage(QSize size)
{
    if (not _ok)
        return;

    _size = size;
    QPixmap pixmap = QPixmap();
    pixmap.loadFromData(_data);
    pixmap = pixmap.transformed(QTransform().rotate(_rotate_angle));
    _label_image->setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImagePreviewDialog::rotate()
{
    if (not _ok)
        return;

    QPixmap pixmap = QPixmap();
    pixmap.loadFromData(_data);
    _rotate_angle += 90 % 360;
    pixmap = pixmap.transformed(QTransform().rotate(_rotate_angle));
    _ratio = pixmap.width() / (double) pixmap.height();
    _label_image->setPixmap(pixmap.scaled(_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void ImagePreviewDialog::moveButtons()
{
    if (this->rect().size().width() < 600 and this->rect().size().height() < 600)
        for (auto &button: {_extend_button, _shrink_button, _rotate_button})
            button->setIconSize(QSize(20, 20));
    else if (this->rect().size().width() < 1000 and this->rect().size().height() < 1000)
        for (auto &button: {_extend_button, _shrink_button, _rotate_button})
            button->setIconSize(QSize(rect().size().width() / 25, rect().size().width() / 25));


    _extend_button->move(this->width() - (_extend_button->iconSize().width() * 2),
                         this->height() - (3 * (_extend_button->iconSize().height() * 2)));
    _shrink_button->move(this->width() - (_shrink_button->iconSize().width() * 2),
                         this->height() - (2 * (_shrink_button->iconSize().height() * 2)));
    _rotate_button->move(this->width() - (_rotate_button->iconSize().width() * 2),
                         this->height() - (_rotate_button->iconSize().height() * 2));

}

void ImagePreviewDialog::resizeEvent(QResizeEvent *event)
{
    moveButtons();
    _timer->start(400);
    QDialog::resizeEvent(event);
}
