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
                                                                                            _remote_info(std::move(
                                                                                                    std::make_shared<RemoteInfo>(
                                                                                                            remoteInfo))),
                                                                                            _deletable(deletable)
{
    init();
}

void RemoteWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QApplication::palette().color(QPalette::Window));
    _click ? painter.setBrush(QApplication::palette().color(QPalette::Window)) : painter.setBrush(
            QApplication::palette().color(QPalette::Mid).lighter(130));

    if (_selection not_eq None)
        // pallet not active
        painter.setBrush(painter.brush().color().darker(140));

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
    if (!_hover)
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

RemoteWidget::RemoteWidget(const RemoteInfoPtr &remoteInfo, bool deletable, QWidget *parent)
{
    _remote_info = remoteInfo;
    this->_deletable = deletable;
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
    if (_remote_info->isLocal())
    {
        icon = Settings::hardDriveIcon();
    } else
        icon = QIcon(_remote_info->icon.c_str());
    labelIcon->setPixmap(icon.pixmap(32, 32, QIcon::Normal, QIcon::On));


    // if length of name is more than 10 chars then cut it
    QString name = QString::fromStdString(_remote_info->name());
    if (name.length() > 10)
        name = name.left(10) + "...";
    auto *labelRemoteName = new QLabel(name);
    labelRemoteName->setToolTip(QString::fromStdString(_remote_info->name()));
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
    _selected_label = new QLabel(this);
    font = _selected_label->font();
    font.setPointSize(15);
    _selected_label->setFont(font);


    auto *_delete = new RoundedButton("✕", this);
    _delete->setFixedSize(25, 25);
    // get max font size for _delete
    auto size = _delete->fontMetrics().boundingRect(_delete->text()).size();
    _delete->setFont(QFont("Arial", size.height() - 5));
    _delete->setToolTip(tr("Supprimer"));

    rightLayout->addWidget(_delete, 0, Qt::AlignTop | Qt::AlignRight);
    rightLayout->addWidget(_selected_label, 0, Qt::AlignBottom | Qt::AlignCenter);

    _layout->addLayout(rightLayout);

    connect(_delete, &RoundedButton::clicked, this, [this]
    {
        if (not _deletable)
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
            if (_remote_info->isLocal())
                Settings::deleteRemote(_remote_info);
            else
            {
                auto rclone = Rclone::create_unique();
                rclone->deleteRemote(_remote_info->name());
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

/**
 * @brief RemoteWidget::setSelection, set selection of remote and update label
 * @param selection
 */
void RemoteWidget::setSelection(Selection selection)
{
    _selection = selection;
    switch (selection)
    {
        case Selection::None:
            _selected_label->setText("");
            break;
        case Selection::First:
            _selected_label->setText("➀");
            break;
        case Selection::Second:
            _selected_label->setText("➁");
            break;
        case Selection::FirstSecond:
            _selected_label->setText("➀➁");
            break;
        default:
            break;
    }
    update();
}