//
// Created by Rahman on 09/01/2023.
//

#include "MainWindow.hpp"

#include <Settings.hpp>
#include <Menu/MenuBar.hpp>
#include <SearchWidget.hpp>
#include <ExplorerWidget.hpp>
#include <IridiumApp.hpp>

#include "InfoWidget.hpp"
#include "SyncWidget.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	Settings::refreshRemotesList();

	setWindowTitle("Iridium");
	setContentsMargins(0, 0, 0, 0);
	resize(Settings::getValue<int>(Settings::Width), Settings::getValue<int>(Settings::Height));
	setMinimumSize(1200, 600);
	QIcon::setThemeName("fluent");

	setMenuBar(new MenuBar(this));

	auto centralWidget = new QWidget(this);
	auto layout = new QVBoxLayout(centralWidget);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	_tabWidget = new QTabWidget(this);
	setCentralWidget(centralWidget);
	_tabWidget->setContentsMargins(0, 0, 0, 0);
	_tabWidget->addTab(new ExplorerWidget(this), tr("Explorateur"));
	_tabWidget->addTab(new SearchWidget(this), tr("Recherche"));
	_tabWidget->addTab(new SyncWidget(this), tr("Synchronisation"));

	layout->addWidget(_tabWidget);

	_bottomWidget = new QWidget(this);
	_bottomLayout = new QHBoxLayout(_bottomWidget);
	_bottomLayout->setContentsMargins(10, 0, 10, 0);
	_bottomWidget->setLayout(_bottomLayout);
	_bottomWidget->setFixedHeight(30);
	_bottomLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_bottomWidget->setContentsMargins(0, 3, 0, 3);

	_info_widget = new InfoWidget(_bottomWidget);
	_bottomLayout->addWidget(_info_widget);

	layout->addWidget(_bottomWidget);

	connectSignals();
}

void MainWindow::connectSignals()
{
	Iridium::Global::signal_add_info.connect([this](std::any info)
	{
		try
		{
			auto widget = std::any_cast<QWidget *>(info);
			widget->layout()->setSizeConstraint(QLayout::SetFixedSize);

			IridiumApp::runOnMainThread([this,widget] { _info_widget->addInfoWidget(widget); });
		}
		catch (const std::bad_any_cast &e) { qDebug() << e.what(); }
	});

	Iridium::Global::signal_remove_info.connect([this](std::any info)
	{
		try
		{
			auto widget = std::any_cast<QWidget *>(info);
			IridiumApp::runOnMainThread([this,widget] { _info_widget->removeInfoWidget(widget); });
		}
		catch (const std::bad_any_cast &e) { qDebug() << e.what(); }
	});
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (_info_widget->taskInProgress())
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Quitter"),
		                                                          tr(
			                                                          "Des taches sont en cours, voulez-vous vraiment quitter ?"),
		                                                          QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No)
		{
			event->ignore();
			return;
		}
	}
	Settings::setValue(std::pair(Settings::Width, width()), std::pair(Settings::Height, height()));
	Settings::saveSettings();
	Settings::deleteAllTempFiles();
	QMainWindow::closeEvent(event);
}
