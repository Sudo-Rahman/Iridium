//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QPushButton>
#include <QLayout>
#include <QProgressBar>
#include "../FileView/TreeWidgets/TreeFileWidget.hpp"
#include "../Remote/AddNewRemote/AddNewRemoteDialog.hpp"
#include "../FileView/TreeWidgets/TreeFileItem.hpp"
#include "../Rclone/RclonesManager.hpp"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(1000, 500);

	auto *wid = new QWidget(this);
	wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	wid->setBackgroundRole(QPalette::Highlight);
	auto *lay = new QHBoxLayout(wid);
	setCentralWidget(wid);

//	Rclone *rclone = new Rclone;
//	pr = new QProgressBar(this);
//	qDebug() <<pr;
//	pr->setMaximum(100);
//	lay->addWidget(new AddNewRemoteDialog);
//	auto t = [this](const int val)
//	{ pr->setValue(val);qDebug() <<val; };
//	rclone->download({"nas:firebase_cpp_sdk_10.3.0.zip", RcloneFile::Distant},{"/Users/sr-71/Downloads/"});
//	connect(rclone, &Rclone::copyProgress,this, t);
//	lay->addWidget(new TreeFileWidget("nass:"));
//	lay->addWidget(new AddNewRemoteDialog);
RclonesManager rr(10);
}
