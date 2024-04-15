//
// Created by Rahman on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QTimer>
#include <Settings.hpp>
#include <Menu/MenuBar.hpp>
#include <SearchWidget.hpp>
#include <ExplorerWidget.hpp>
#include <SyncWidget.hpp>
#include <IridiumApp.hpp>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    Settings::refreshRemotesList();

    setWindowTitle("Iridium");
    setContentsMargins(0, 0, 0, 0);
    resize(Settings::getValue<int>(Settings::Width), Settings::getValue<int>(Settings::Height));
    setMinimumSize(1200, 600);
    QIcon::setThemeName("fluent");

    setMenuBar(new MenuBar(this));

    _tabWidget = new QTabWidget(this);
    setCentralWidget(_tabWidget);
    _tabWidget->setContentsMargins(0, 0, 0, 0);
    _tabWidget->addTab(new ExplorerWidget(this), tr("Explorateur"));
    _tabWidget->addTab(new SearchWidget(this), tr("Recherche"));
//    _tabWidget->addTab(new SyncWidget(this), tr("Synchronisation"));


    _statusBar = new QStatusBar(this);
    setStatusBar(_statusBar);
    _statusBar->setLayoutDirection(Qt::RightToLeft);
    _statusBar->setContentsMargins(10, 0, 10, 0);

    connectSignals();

}

void MainWindow::connectSignals()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::setValue(Settings::Width, width(), Settings::Height, height());
    Settings::saveSettings();
    QMainWindow::closeEvent(event);
}