//
// Created by sr-71 on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QProgressBar>
#include <QCoreApplication>
#include <QSettings>
#include "../Rclone/Rclone.hpp"
#include "../Remote/ListRemote/ListRemoteWidget.hpp"
#include "../FileView/FileViewWidget.hpp"
#include "ToolBar.hpp"


class MainWindow : public QMainWindow
{
Q_OBJECT
	QList<RemoteWidget *> m_listRemote{};
	ToolBar *m_toolBar{};
	ListRemoteWidget *m_listRemoteWidget{};
	FileViewWidget *m_fileViewWidget{};


public:
	explicit MainWindow(QWidget *parent = nullptr);

private:
	void connectSignals();
};


#endif //IRIDIUM_MAINWINDOW_HPP
