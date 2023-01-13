//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_CHOSEREMOTEDIALOG_HPP
#define IRIDIUM_CHOSEREMOTEDIALOG_HPP

#include <QDialog>
#include "../Rclone/Rclone.hpp"


class ChoseRemoteDialog : public QDialog
{
Q_OBJECT
    Rclone * rclone {};
public:
    explicit ChoseRemoteDialog( Rclone * rclone, QWidget * parent = nullptr );


};


#endif //IRIDIUM_CHOSEREMOTEDIALOG_HPP
