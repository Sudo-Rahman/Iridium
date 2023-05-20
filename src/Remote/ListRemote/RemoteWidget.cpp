//
// Created by Rahman on 29/03/2023.
//

#include "RemoteWidget.hpp"
#include <Settings.hpp>

#include <QPainter>
#include <QEvent>
#include <QPropertyAnimation>
#include <utility>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <Rclone.hpp>

RemoteWidget::RemoteWidget(const RemoteInfo &remoteInfo, bool deletable, QWidget *parent) : QGroupBox(parent),
                                                                                            m_remote_info(std::move(
                                                                                                    std::make_shared<RemoteInfo>(
                                                                                                            remoteInfo))),
                                                                                            m_deletable(deletable)
{
    init();
}

void RemoteWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QApplication::palette().color(QPalette::Window));
    m_click ? painter.setBrush(QApplication::palette().color(QPalette::Window)) : painter.setBrush(
            QApplication::palette().color(QPalette::Mid).lighter(130));

    // draw rounded rect
    QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 10, 10);

}

bool RemoteWidget::event(QEvent *event)
{
    switch (event->type())
    {
        // mouse hover repaint
        case QEvent::Enter:
            m_hover = true;
            addBlur();
            break;
        case QEvent::Leave:
            m_hover = false;
            addBlur();
            break;
        case QEvent::MouseButtonPress:
            // change cursor
            m_click = true;
            repaint();
            setCursor(Qt::PointingHandCursor);
            break;
        case QEvent::MouseButtonRelease:
            // change cursor
            m_click = false;
            repaint();
            setCursor(Qt::ArrowCursor);
            emit clicked(this);
            break;
        default:
            break;
    }
    return QGroupBox::event(event);
}

void RemoteWidget::addBlur()
{
    // if not hover remove effect
    if (!m_hover)
    {
        this->setGraphicsEffect(nullptr);
        return;
    }
    // resize effect
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(20);
    effect->setOffset(0, 0);
    effect->setColor(QApplication::palette().color(QPalette::Dark));
    this->setGraphicsEffect(effect);
}

const RemoteInfoPtr &RemoteWidget::remoteInfo() const
{
    return m_remote_info;
}

void RemoteWidget::setSelectedText(const QString &text)
{
    m_selected->setText(text);
}

RemoteWidget::RemoteWidget(const RemoteInfoPtr &remoteInfo, bool deletable, QWidget *parent)
{
    m_remote_info = remoteInfo;
    this->m_deletable = deletable;
    init();
}

void RemoteWidget::init()
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(10, 5, 5, 5);
    _layout->setSpacing(0);

    auto *labelIcon = new QLabel;
    _layout->addWidget(labelIcon);
// create pixmap
    QIcon icon;
    if (m_remote_info->isLocal())
    {
        icon = Settings::hardDriveIcon();
    } else
        icon = QIcon(m_remote_info->icon.c_str());
    labelIcon->setPixmap(icon.pixmap(32, 32, QIcon::Normal, QIcon::On));


    // if length of name is more than 10 chars then cut it
    QString name = QString::fromStdString(m_remote_info->name());
    if (name.length() > 10)
        name = name.left(10) + "...";
    auto *labelRemoteName = new QLabel(name);
    labelRemoteName->setToolTip(QString::fromStdString(m_remote_info->name()));
    labelRemoteName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    labelRemoteName->setAlignment(Qt::AlignCenter);
    // set auto size font
    QFont font = labelRemoteName->font();
    font.setPointSize(15);
    labelRemoteName->setFont(font);
    _layout->addWidget(labelRemoteName);

    auto rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    m_selected = new QLabel(this);


    auto *m_delete = new RoundedButton("✕", this);
    m_delete->setFixedSize(25, 25);
    // get max font size for m_delete
    auto size = m_delete->fontMetrics().boundingRect(m_delete->text()).size();
    m_delete->setFont(QFont("Arial", size.height() - 5));
    m_delete->setToolTip(tr("Supprimer"));

    rightLayout->addWidget(m_delete, 0, Qt::AlignTop | Qt::AlignRight);
    rightLayout->addWidget(m_selected, 0, Qt::AlignBottom | Qt::AlignCenter);

    _layout->addLayout(rightLayout);

    connect(m_delete, &RoundedButton::clicked, this, [this]
    {
        if (not m_deletable)
        {
            QMessageBox::warning(this, tr("Suppression"), tr("Vous ne pouvez pas supprimer ce remote."));
            return;
        }
        auto msgbox = QMessageBox(QMessageBox::Question, tr("Suppression"),
                                  tr("Êtes-vous sûr de vouloir supprimer ce remote ?"));
        auto yes = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
        auto no = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
        msgbox.exec();

        if (msgbox.clickedButton() == yes)
        {
            if (m_remote_info->isLocal())
                Settings::deleteRemote(m_remote_info);
            else
            {
                auto rclone = RcloneManager::get();
                rclone->deleteRemote(m_remote_info->name());
                rclone->waitForFinished();
                if (rclone->exitCode() != 0)
                {
                    auto msgb = QMessageBox(QMessageBox::Critical, tr("Suppression"),
                                            tr("Une erreur est survenue lors de la suppression du remote"),
                                            QMessageBox::Ok, this);
                    msgb.setDetailedText(QString::fromStdString(rclone->readAllError().back()));
                    msgb.exec();
                    return;
                }
            }
            emit deleted(this);
        }
    });
}