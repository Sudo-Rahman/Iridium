//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QLayout>
#include <QTimer>
#include <Settings.hpp>
#include <Menu/MenuBar.hpp>
#include <TaskTreeView.hpp>
#include <ProgressBar.hpp>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(1000, 500);
	QIcon::setThemeName("fluent");


	setMenuBar(new MenuBar(this));

	auto *wid = new QWidget(this);
	wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_layout = new QHBoxLayout(wid);
	m_layout->setContentsMargins(0, 10, 10, 10);
	setCentralWidget(wid);

	m_listRemoteWidget = new ListRemoteWidget(this);
	m_layout->addWidget(m_listRemoteWidget);

	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);
	m_splitter->setHandleWidth(15);
	// no collapsible first widget
	m_layout->addWidget(m_splitter);

	m_fileViewWidget = new FileViewWidget(this);
	m_splitter->addWidget(m_fileViewWidget);
	m_fileViewWidget->changeRemote(m_listRemoteWidget->remoteSelected());

	m_taskTreeView = new TaskTreeView(this);
	m_splitter->addWidget(m_taskTreeView);

	m_splitter->setCollapsible(0, false);
	m_splitter->setCollapsible(1, true);
	// collapse second widget
	m_splitter->setSizes({10000, 0});

	connectSignals();
}

void MainWindow::connectSignals()
{
	connect(m_listRemoteWidget, &ListRemoteWidget::remoteClicked, this,
			[this](const std::shared_ptr<remotes_selected> &remotes)
			{
				m_fileViewWidget->changeRemote(remotes);
			});
	connect(m_fileViewWidget, &FileViewWidget::taskAdded, this,
			[this](const QString &src, const QString &dst, const RclonePtr &rclone,
				   const std::function<void()> &callable, const Rclone::TaskType &type)
			{
				if (m_splitter->sizes()[1] == 0)
					m_splitter->setSizes({1000, 400});
				m_taskTreeView->addTask(src, dst, rclone, callable, type);
			});

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	Settings::saveSettings();
	QMainWindow::closeEvent(event);
}