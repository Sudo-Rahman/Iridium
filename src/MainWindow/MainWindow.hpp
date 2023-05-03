//
// Created by Rahman on 09/01/2023.
//

#ifndef IRIDIUM_MAINWINDOW_HPP
#define IRIDIUM_MAINWINDOW_HPP

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QLayout>
#include <QSplitter>
#include <Rclone.hpp>
#include <ListRemoteWidget.hpp>
#include <FileViewWidget.hpp>
#include <TaskTreeView.hpp>


class MainWindow : public QMainWindow
{
Q_OBJECT

    ListRemoteWidget *m_list_remoteWidget{};
    FileViewWidget *m_file_viewWidget{};
    QHBoxLayout *m_layout{};
    QSplitter *m_splitter{};
    TaskTreeView *m_task_treeView{};

public:
    explicit MainWindow(QWidget *parent = nullptr);


private:
    void connectSignals();

protected:
    void closeEvent(QCloseEvent *event) override;
};


#endif //IRIDIUM_MAINWINDOW_HPP
