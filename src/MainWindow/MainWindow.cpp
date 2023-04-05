//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QPushButton>
#include <QLayout>
#include <QTimer>
#include <QMenuBar>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(1000, 500);
	QIcon::setThemeName("fluent");

	// set tool bar
	m_toolBar = new ToolBar(this);
	addToolBar(m_toolBar);

	auto *wid = new QWidget(this);
	wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	auto *lay = new QHBoxLayout(wid);
	setCentralWidget(wid);

	m_listRemoteWidget = new ListRemoteWidget(this);
	lay->addWidget(m_listRemoteWidget);


	m_fileViewWidget = new FileViewWidget(RemoteInfo("nas2:", RemoteType::Drive), this);
	lay->addWidget(m_fileViewWidget);

	connectSignals();
}

void MainWindow::connectSignals()
{
	connect(m_toolBar, &ToolBar::hideListeRemotes, this, [=]()
	{
		m_listRemoteWidget->expand();
	});

	connect(m_listRemoteWidget, &ListRemoteWidget::remoteClicked, this, [=](RemoteWidget *remote)
	{
		m_fileViewWidget->changeRemote(remote->remoteInfo());
	});
}
