//
// Created by Rahman on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QCoreApplication>
#include <QLayout>
#include <QTabWidget>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
Q_OBJECT

    QHBoxLayout *_layout{};
    QTabWidget *_tabWidget{};
    QStatusBar *_statusBar{};

    bool _check_rclone = false;

public:
    explicit MainWindow(QWidget *parent = nullptr);


private:
    void connectSignals();

//    void downloadRclone();
protected:
    void closeEvent(QCloseEvent *event) override;
};


#endif //IRIDIUM_MAINWINDOW_HPP
