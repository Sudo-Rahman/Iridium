//
// Created by Rahman on 11/01/2023.
//

#ifndef IRIDIUM_ADDNEWREMOTEDIALOG_HPP
#define IRIDIUM_ADDNEWREMOTEDIALOG_HPP

#include <QDialog>
#include <QLayout>
#include <Rclone.hpp>
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


#endif //IRIDIUM_ADDNEWREMOTEDIALOG_HPP
