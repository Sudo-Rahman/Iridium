//
// Created by Rahman on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QCoreApplication>
#include <QLayout>
#include <QTabWidget>


class MainWindow : public QMainWindow
{
Q_OBJECT

    QHBoxLayout *_layout{};
    QTabWidget *_tabWidget{};

public:
    explicit MainWindow(QWidget *parent = nullptr);


private:
    void connectSignals();

protected:
    void closeEvent(QCloseEvent *event) override;
};


#endif //IRIDIUM_MAINWINDOW_HPP
