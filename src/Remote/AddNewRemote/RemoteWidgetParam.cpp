//
// Created by Rahman on 11/01/2023.
//

#include "RemoteWidgetParam.hpp"
#include "RemoteConfigParamsFrame.hpp"

#include "GoogleDrive/GoogleDriveRemoteConfigParamsFrame.hpp"
#include "Local/LocalRemoteConfigParamsFrame.hpp"
#include "Sftp/SftpRemoteConfigParamsFrame.hpp"
#include "OneDrive/OneDriveRemoteConfigParamsFrame.hpp"
#include "DropBox/DropboxRemoteConfigParamsFrame.hpp"
#include "Ftp/FtpRemoteConfigParamsFrame.hpp"
#include "Mega/MegaRemoteConfigParamsFrame.hpp"
#include "OpenDrive/OpenDriveRemoteConfigParamsFrame.hpp"
#include "Pcloud/PcloudRemoteConfigParamsFrame.hpp"
#include "Box/BoxRemoteConfigParamsFrame.hpp"
#include "Smb/SmbRemoteConfigParamsFrame.hpp"
#include "Cmd/CmdRemoteConfigParamsFrame.hpp"
#include <Settings.hpp>

#include <QLayout>
#include <QLabel>
#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <QGraphicsDropShadowEffect>


RemoteWidgetParam::RemoteWidgetParam(ire::remote::remote_type type, QWidget *parent) :
        QGroupBox(parent), _type(type)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setContentsMargins(10, 10, 10, 10);

    initParamsFrame();

    auto *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->setSpacing(15);

    auto *labelIcon = new QLabel(this);
    layout->addWidget(labelIcon);


    auto *labelRemoteName = new QLabel(this);
    layout->addWidget(labelRemoteName);
    switch (_type)
    {
        case ire::remote::google_drive:
            labelRemoteName->setText("Google Drive");
            break;
        case ire::remote::sftp:
            labelRemoteName->setText("Sftp");
            break;
        case ire::remote::none:
            labelRemoteName->setText("Local");
            break;
        case ire::remote::onedrive:
            labelRemoteName->setText("OneDrive");
            break;
        case ire::remote::dropbox:
            labelRemoteName->setText("Dropbox");
            break;
        case ire::remote::ftp:
            labelRemoteName->setText("Ftp");
            break;
        case ire::remote::mega:
            labelRemoteName->setText("Mega");
            break;
        case ire::remote::opendrive:
            labelRemoteName->setText("OpenDrive");
            break;
        case ire::remote::pcloud:
            labelRemoteName->setText("Pcloud");
            break;
        case ire::remote::box:
            labelRemoteName->setText("Box");
            break;
        case ire::remote::smb:
            labelRemoteName->setText("Smb");
            break;
        case ire::remote::cmd:
            labelRemoteName->setText(tr("Manuel"));
            break;
        default:
            break;
    }
    if (type == ire::remote::none)
        _icon = Settings::hardDriveIcon();
    else
        _icon = QIcon(QString::fromStdString(remoteIco.find(RemoteWidgetParam::_type)->second));
    labelIcon->setPixmap(_icon.pixmap(32, 32, QIcon::Normal, QIcon::On));

    update();
}

void RemoteWidgetParam::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QGroupBox::palette().color(QPalette::Light));
    _click ? painter.setBrush(QApplication::palette().color(QPalette::Window)) : painter.setBrush(
            QApplication::palette().color(QPalette::Mid).lighter(130));
    if (_selected)
        // darker color
        painter.setBrush(painter.brush().color().darker(140));

    // draw rounded rect
    QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 10, 10);
}

void RemoteWidgetParam::initParamsFrame()
{
    switch (_type)
    {
        case ire::remote::google_drive:
            _paramsFrame = new GoogleDriveRemoteConfigParamsFrame();
            break;
        case ire::remote::sftp:
            _paramsFrame = new SftpRemoteConfigParamsFrame();
            break;
        case ire::remote::none:
            _paramsFrame = new LocalRemoteConfigParamsFrame();
            break;
        case ire::remote::onedrive:
            _paramsFrame = new OneDriveRemoteConfigParamsFrame();
            break;
        case ire::remote::dropbox:
            _paramsFrame = new DropboxRemoteConfigParamsFrame();
            break;
        case ire::remote::ftp:
            _paramsFrame = new FtpRemoteConfigParamsFrame();
            break;
        case ire::remote::mega:
            _paramsFrame = new MegaRemoteConfigParamsFrame();
            break;
        case ire::remote::opendrive:
            _paramsFrame = new OpenDriveRemoteConfigParamsFrame();
            break;
        case ire::remote::pcloud:
            _paramsFrame = new PcloudRemoteConfigParamsFrame();
            break;
        case ire::remote::box:
            _paramsFrame = new BoxRemoteConfigParamsFrame();
            break;
        case ire::remote::smb:
            _paramsFrame = new SmbRemoteConfigParamsFrame();
            break;
        case ire::remote::cmd:
            _paramsFrame = new CmdRemoteConfigParamsFrame();
            break;
        default:
            break;
    }
    connect(_paramsFrame, &RemoteConfigParamsFrame::remoteAdded, this, &RemoteWidgetParam::newRemoteAdded);
}

RemoteConfigParamsFrame *RemoteWidgetParam::getParamsFrame() const
{
    return _paramsFrame;
}

bool RemoteWidgetParam::event(QEvent *event)
{
    switch (event->type())
    {
        // mouse hover repaint
        case QEvent::Enter:
            _hover = true;
            addBlur();
            break;
        case QEvent::Leave:
            _hover = false;
            addBlur();
            break;
        case QEvent::MouseButtonPress:
            // change cursor
            _click = true;
            repaint();
            setCursor(Qt::PointingHandCursor);
            break;
        case QEvent::MouseButtonRelease:
            // change cursor
            _click = false;
            repaint();
            setCursor(Qt::ArrowCursor);
            emit clicked(_paramsFrame);
            break;
        default:
            break;
    }
    return QGroupBox::event(event);
}

void RemoteWidgetParam::addBlur()
{
    // if not hover remove effect
    if (not _hover)
    {
        this->setGraphicsEffect(nullptr);
        return;
    }
    // resize effect
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(20);
    effect->setOffset(0, 0);
    effect->setColor(QWidget::palette().color(QPalette::Dark));
    this->setGraphicsEffect(effect);
}
