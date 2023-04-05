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


class MainWindow : public QMainWindow
{
Q_OBJECT
	QList<RemoteWidget *> m_listRemote{};
public:
	explicit MainWindow(QWidget *parent = nullptr);

};


#endif //IRIDIUM_MAINWINDOW_HPP
