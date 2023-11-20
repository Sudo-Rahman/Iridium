//
// Created by Rahman on 29/03/2023.
//

#ifndef IRIDIUM_LISTREMOTEWIDGET_HPP
#define IRIDIUM_LISTREMOTEWIDGET_HPP

#include <QScrollArea>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include <QEvent>

#include "RemoteWidget.hpp"
#include <Remote.h>
#include <RoundedButton.hpp>
#include <RoundedLineEdit.hpp>
#include <Rclone.hpp>
#include <boost/asio/io_service.hpp>

struct remotes_selected
{
    RemoteWidget *first{};
    RemoteWidget *second{};

    void clear()
    {
        first = nullptr;
        second = nullptr;
    }
};

class ListRemoteWidget : public QScrollArea
{
Q_OBJECT

private:
    QVBoxLayout *_layout{}, *_remote_layout{};

    RoundedLineEdit *_recherche{};
    RoundedButton *_add{};

    RclonePtr _rclone{};

    QList<RemoteWidget *> _remotes{};

    std::shared_ptr<remotes_selected> _remoteselected{};

    QPushButton *_expand{};

    bool _selected{};


    int _width{};
    bool _is_expand = true;


public:
    explicit ListRemoteWidget(QWidget *parent = nullptr);

    void expand();

    const std::shared_ptr<remotes_selected> &remoteSelected() { return _remoteselected; }

private:
    void getAllRemote();

    void searchRemote(const QString &name);

    void showAnimation(QWidget *widget) const;

    void hideAnimation(QWidget *widget) const;

protected:
    bool event(QEvent *event) override;

signals:

    void remoteClicked(const std::shared_ptr<remotes_selected> &);
};

class RefreshRemoteEvent : public QEvent
{
public:
    static const QEvent::Type refreshRemoteType = static_cast<QEvent::Type>(300);
};

#endif //IRIDIUM_LISTREMOTEWIDGET_HPP
