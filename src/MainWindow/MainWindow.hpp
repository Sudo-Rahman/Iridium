//
// Created by sr-71 on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QProgressBar>
#include <QCoreApplication>
#include "../Rclone/Rclone.hpp"

class MainWindow : public QMainWindow
{
Q_OBJECT
	QProgressBar *pr;
public:
	explicit MainWindow(QWidget *parent = nullptr);

};


#endif //IRIDIUM_MAINWINDOW_HPP
