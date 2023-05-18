//
// Created by Rahman on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QTimer>
#include <Settings.hpp>
#include <Menu/MenuBar.hpp>
#include "SearchWidget.hpp"
#include <ExplorerWidget.hpp>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Iridium");
    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(1200, 600);
    QIcon::setThemeName("fluent");


    setMenuBar(new MenuBar(this));


    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    m_tabWidget->setContentsMargins(0, 0, 0, 0);
    m_tabWidget->addTab(new ExplorerWidget(this), tr("Explorateur"));
    m_tabWidget->addTab(new SearchWidget(this), tr("Recherche"));


    connectSignals();
}

void MainWindow::connectSignals()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::saveSettings();
    QMainWindow::closeEvent(event);
    RcloneManager::stopThread();
}