//
// Created by sr-71 on 15/04/2024.
//

#pragma once

#include <boost/thread.hpp>

#include "AddNewRemoteDialog.hpp"

class RcloneNotFoundWidget : public QDialog {
	Q_OBJECT

public:
	explicit RcloneNotFoundWidget(QWidget *parent = nullptr);

	auto rcloneInstalled() const -> bool { return _downloaded; }


private:
	QLineEdit *_rclone_path;
	QPushButton *_ok_button,*_download;
	boost::thread _thread;
	bool _downloaded = false;

	void downloadRclone();
};

