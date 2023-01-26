//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_ADDNEWREMOTEDIALOG_HPP
#define IRIDIUM_ADDNEWREMOTEDIALOG_HPP

#include <QDialog>
#include <QLayout>
#include "../../Rclone/Rclone.hpp"
#include "RemoteFrame/RemoteConfigParamsFrame.hpp"


class AddNewRemoteDialog : public QDialog
{
Q_OBJECT
	QHBoxLayout *layout{};
	RemoteConfigParamsFrame *paramsFrame{};

public:
	explicit AddNewRemoteDialog(QWidget *parent = nullptr);

private:
	void changeParamsFrame(RemoteConfigParamsFrame *);


};


#endif //IRIDIUM_ADDNEWREMOTEDIALOG_HPP
