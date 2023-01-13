//
// Created by sr-71 on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QCoreApplication>

class MainWindow : public QMainWindow
{
Q_OBJECT
    const QString rclonePath = QCoreApplication::applicationDirPath().append( "/rclone" );
public:
    explicit MainWindow( QWidget * parent = nullptr );

};


#endif //IRIDIUM_MAINWINDOW_HPP
