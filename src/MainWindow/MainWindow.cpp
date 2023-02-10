//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"
#include "../FileView/TreeWidgets/TreeFileView.hpp"

#include <QPushButton>
#include <QLayout>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(1000, 500);
	QIcon::setThemeName( "fluent" );


	auto *wid = new QWidget(this);
	wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	wid->setBackgroundRole(QPalette::Highlight);
	auto *lay = new QHBoxLayout(wid);
	setCentralWidget(wid);

	auto *tree = new TreeFileView(Remote::Distant, "nass:");
	lay->addWidget(tree);
	auto *bt = new QPushButton("arrier");
	lay->addWidget(bt);
	auto *bt1 = new QPushButton("avant");
	lay->addWidget(bt1);
	connect(bt, &QPushButton::clicked, tree, &TreeFileView::back);
	connect(bt1, &QPushButton::clicked, tree, &TreeFileView::front);
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
}
