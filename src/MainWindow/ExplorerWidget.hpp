//
// Created by sr-71 on 09/05/2023.
//

#ifndef IRIDIUM_EXPLORERWIDGET_HPP
#define IRIDIUM_EXPLORERWIDGET_HPP

#include <QLayout>
#include <QSplitter>
#include <QTabWidget>
#include <Rclone.hpp>
#include <ListRemoteWidget.hpp>
#include <FileViewWidget.hpp>
#include <TaskTreeView.hpp>


class ExplorerWidget : public QWidget
{
Q_OBJECT

    ListRemoteWidget *m_list_remoteWidget{};
    FileViewWidget *m_file_viewWidget{};
    QHBoxLayout *m_layout{};
    QSplitter *m_splitter{};
    TaskTreeView *m_task_treeView{};

public:
    explicit ExplorerWidget(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_EXPLORERWIDGET_HPP
