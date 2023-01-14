//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_CHOSEREMOTEDIALOG_HPP
#define IRIDIUM_CHOSEREMOTEDIALOG_HPP

#include <QDialog>
#include <QLayout>
#include "../../Rclone/Rclone.hpp"
#include "RemoteFrame/RemoteConfigParamsFrame.hpp"


class ChoseRemoteDialog : public QDialog
{
Q_OBJECT
    QHBoxLayout *layout{};
    RemoteConfigParamsFrame *paramsFrame{};

public:
    explicit ChoseRemoteDialog(QWidget *parent = nullptr);

private:
    void changeParamsFrame(RemoteConfigParamsFrame *);


};


#endif //IRIDIUM_CHOSEREMOTEDIALOG_HPP
