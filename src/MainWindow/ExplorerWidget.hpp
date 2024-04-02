//
// Created by sr-71 on 09/05/2023.
//

#pragma once

#include <ListRemoteWidget.hpp>
#include <FileViewWidget.hpp>
#include <TaskTreeView.hpp>


class ExplorerWidget : public QWidget
{
Q_OBJECT

    ListRemoteWidget *_list_remoteWidget{};
    FileViewWidget *_file_viewWidget{};
    QHBoxLayout *_layout{};
    QSplitter *_splitter{};
    TaskTreeView *_task_treeView{};

public:
    explicit ExplorerWidget(QWidget *parent = nullptr);

private:
    void connectSignals();

};
