//
// Created by sr-71 on 09/05/2023.
//

#include "MainWidget.hpp"


MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 0, 5, 5);

    m_list_remoteWidget = new ListRemoteWidget(this);
    m_layout->addWidget(m_list_remoteWidget);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->setHandleWidth(15);
    // no collapsible first widget
    m_layout->addWidget(m_splitter);

    m_file_viewWidget = new FileViewWidget(this);
    m_splitter->addWidget(m_file_viewWidget);
    m_file_viewWidget->changeRemote(m_list_remoteWidget->remoteSelected());

    m_task_treeView = new TaskTreeView(this);
    m_splitter->addWidget(m_task_treeView);

    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, true);
    // collapse second widget
    m_splitter->setSizes({10000, 0});

    connectSignals();
}

void MainWidget::connectSignals()
{
    connect(m_list_remoteWidget, &ListRemoteWidget::remoteClicked, this,
            [this](const std::shared_ptr<remotes_selected> &remotes)
            {
                m_file_viewWidget->changeRemote(remotes);
            });
    connect(m_file_viewWidget, &FileViewWidget::taskAdded, this,
            [this](const QString &src, const QString &dst, const RclonePtr &rclone,
                   const std::function<void()> &callable, const Rclone::TaskType &type)
            {
                if (m_splitter->sizes()[1] == 0)
                    m_splitter->setSizes({1000, 400});
                m_task_treeView->addTask(src, dst, rclone, callable, type);
            });
}