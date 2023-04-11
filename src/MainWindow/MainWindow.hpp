//
// Created by sr-71 on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QLayout>
#include <Rclone.hpp>
#include <ListRemoteWidget.hpp>
#include <FileViewWidget.hpp>
#include "ToolBar.hpp"


class MainWindow : public QMainWindow
{
Q_OBJECT

	QList<RemoteWidget *> m_listRemote{};
	ToolBar *m_toolBar{};
	ListRemoteWidget *m_listRemoteWidget{};
	FileViewWidget *m_fileViewWidget{};
	QHBoxLayout *m_layout{};
	QVBoxLayout *m_mainLayout{};

public:
	explicit MainWindow(QWidget *parent = nullptr);

private:
	void connectSignals();
};


#endif //IRIDIUM_MAINWINDOW_HPP
