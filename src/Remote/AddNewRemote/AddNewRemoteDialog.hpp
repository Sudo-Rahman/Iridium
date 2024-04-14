//
// Created by Rahman on 11/01/2023.
//

#pragma once

#include <QDialog>
#include "RemoteConfigParamsFrame.hpp"


class AddNewRemoteDialog : public QDialog
{
Q_OBJECT

    QHBoxLayout *_layout{};
    RemoteConfigParamsFrame *_paramsFrame{};

public:
    explicit AddNewRemoteDialog(QWidget *parent = nullptr);

private:
    void changeParamsFrame(RemoteConfigParamsFrame *);

signals:

    void newRemoteAdded();

};