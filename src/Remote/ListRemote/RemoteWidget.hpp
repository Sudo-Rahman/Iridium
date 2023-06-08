//
// Created by Rahman on 29/03/2023.
//

#ifndef IRIDIUM_REMOTEWIDGET_HPP
#define IRIDIUM_REMOTEWIDGET_HPP

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <Remote.h>
#include <RoundedButton.hpp>

class RemoteWidget : public QGroupBox
{
Q_OBJECT

public:

    enum Selection
    {
        First, Second, FirstSecond, None
    };

protected:
    QHBoxLayout *_layout{};

    RemoteInfoPtr _remote_info{};
    QLabel *_selected_label{};

    RoundedButton *_delete{};

    bool _hover{}, _click{};

    bool _deletable = true;

    Selection _selection = None;

protected:
    void paintEvent(QPaintEvent *event) override;

    virtual bool event(QEvent *event) override;

private:
    void init();

    void addBlur();

public:

    explicit RemoteWidget(const RemoteInfo &remoteInfo, bool deletable = true, QWidget *parent = nullptr);

    explicit RemoteWidget(const RemoteInfoPtr &remoteInfo, bool deletable = true, QWidget *parent = nullptr);

    [[nodiscard]] const RemoteInfoPtr &remoteInfo() const { return _remote_info; }

    bool operator==(const RemoteWidget &other) const { return _remote_info == other._remote_info; }

    void setSelection(Selection selection);

signals:

    void clicked(RemoteWidget *);

    void deleted(RemoteWidget *);
};


#endif //IRIDIUM_REMOTEWIDGET_HPP

