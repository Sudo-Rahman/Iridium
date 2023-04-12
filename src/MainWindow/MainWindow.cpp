//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QLayout>
#include <QTimer>
#include <QMenuBar>
#include <TaskTreeView.hpp>


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
	m_layout = new QHBoxLayout(wid);
	setCentralWidget(wid);

	m_listRemoteWidget = new ListRemoteWidget(this);
	m_layout->addWidget(m_listRemoteWidget);

	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);
	// no collapsible first widget
	m_layout->addWidget(m_splitter);

	m_fileViewWidget = new FileViewWidget(this);
	m_splitter->addWidget(m_fileViewWidget);
	m_fileViewWidget->changeRemote(m_listRemoteWidget->remoteSelected());

	m_splitter->addWidget(new TaskTreeView(this));

	m_splitter->setCollapsible(0, false);
	m_splitter->setCollapsible(1, true);
	// collapse second widget
	m_splitter->setSizes({10000,  0 });

	connectSignals();
}

void MainWindow::connectSignals()
{
	connect(m_toolBar, &ToolBar::hideListeRemotes, this, [this]()
	{
		m_listRemoteWidget->expand();
	});

	connect(m_listRemoteWidget, &ListRemoteWidget::remoteClicked, this,
			[this](const std::shared_ptr<remotes_selected> &remotes)
			{
				m_fileViewWidget->changeRemote(remotes);
			});
	connect(m_fileViewWidget, &FileViewWidget::progressBarCreated, this, [this](QProgressBar *progressBar)
	{
	});
}
