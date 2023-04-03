//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"
#include "../FileView/FileViewWidget.hpp"

#include <QPushButton>
#include <QLayout>
#include <QTimer>
#include <QMenuBar>

#include "../Remote/ListRemote/ListRemoteWidget.hpp"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(1000, 500);
	QIcon::setThemeName("fluent");


	auto *wid = new QWidget(this);
	wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	auto *lay = new QHBoxLayout(wid);
	setCentralWidget(wid);

	auto *listRemote = new ListRemoteWidget(this);
	lay->addWidget(listRemote);

	auto *tree = new FileViewWidget(RemoteInfo("nas2:",RemoteType::Drive), this);
	lay->addWidget(tree);

//	pr = new QProgressBar(this);
//	lay->addWidget(pr);
	auto *rclone = new Rclone;
//	auto *timer = new QTimer(this);
//	timer->setInterval(10);
//	connect(timer, &QTimer::timeout, this, [=]()
//	{
//		if (pr->value() < 100)
//			pr->setValue(pr->value() + 1);
//		else
//			pr->setValue(0);
//	});
//	timer->start();
//	rclone->copyTo({"nas:firebase_cpp_sdk_10.3.0.zip", RcloneFile::Distant},{"/Users/sr-71/Downloads/"});
//	connect(rclone, &Rclone::copyProgress,this, [=](const int val){
//		pr->setValue(val);
//	});
//	lay->addWidget(new AddNewRemoteDialog);

	connect(listRemote, &ListRemoteWidget::remoteClicked, this, [=](RemoteWidget *remote)
	{
		tree->changeRemote(remote->remoteInfo());
	});

}
