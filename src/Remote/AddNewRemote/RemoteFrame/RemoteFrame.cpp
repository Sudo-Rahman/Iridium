//
// Created by sr-71 on 11/01/2023.
//

#include "RemoteFrame.hpp"
#include "RemoteConfigParamsFrame.hpp"

#include "GoogleDrive/GoogleDriveRemoteConfigParamsFrame.hpp"

#include <QLayout>
#include <QLabel>
#include <QStyle>
#include <QPainter>

RemoteFrame::RemoteFrame(RemoteType type, QWidget *parent) :
        QFrame(parent), type(type)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setContentsMargins(10, 10, 10, 10);

    initParamsFrame();

    auto *layout = new QHBoxLayout(this);
    layout->setSpacing(20);

    auto *labelIcon = new QLabel;
    labelIcon->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelIcon);


    auto *labelRemoteName = new QLabel;
    labelRemoteName->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelRemoteName);
    switch (RemoteFrame::type)
    {
        case Drive:
            labelRemoteName->setText("Google Drive");
            image = {":/images/google_drive.png"};
            break;
        case Sftp:
            labelRemoteName->setText("Sftp");
            image = {":/images/sftp.png"};
            break;
    }
    labelIcon->setPixmap(image.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    update();
}

RemoteType RemoteFrame::getType() const
{
    return type;
}

void RemoteFrame::setType(RemoteType type)
{
    RemoteFrame::type = type;
}

void RemoteFrame::mousePressEvent(QMouseEvent *event)
{
    QFrame::mousePressEvent(event);
    clickRelease = !clickRelease;
    update();
}

void RemoteFrame::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);
    clickRelease = !clickRelease;
    update();
    emit clicked(paramsFrame);
}

void RemoteFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (clickRelease)
        painter.setBrush(QColor(113, 121, 126));
    else
        painter.setBrush(QBrush());
    painter.setPen(Qt::gray);

    QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 15, 15);

    QFrame::paintEvent(event);
}

void RemoteFrame::initParamsFrame()
{
    switch (type)
    {

        case Drive:
            paramsFrame = new GoogleDriveRemoteConfigParamsFrame();
        case Sftp:
            break;
    }

}

RemoteConfigParamsFrame *RemoteFrame::getParamsFrame() const
{
    return paramsFrame;
}
