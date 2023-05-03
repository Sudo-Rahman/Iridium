//
// Created by Rahman on 30/03/2023.
//

#include "FileViewWidget.hpp"

FileViewWidget::FileViewWidget(QWidget *parent) : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    setContentsMargins(0, 0, 0, 0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_layout->addWidget(m_splitter);

    m_treeFileView1 = new TreeFileViewContainer(this);
    m_splitter->addWidget(m_treeFileView1);

    m_treeFileView2 = new TreeFileViewContainer(this);
    m_splitter->addWidget(m_treeFileView2);

    connect(m_treeFileView1->treeFileView(), &TreeFileView::fileCopied, this,
            [this](const QList<TreeFileItem *> &lst) { m_current_file_list = lst; });
    connect(m_treeFileView2->treeFileView(), &TreeFileView::fileCopied, this,
            [this](const QList<TreeFileItem *> &lst) { m_current_file_list = lst; });
    connect(m_treeFileView1->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
    {
        if (m_current_file_list.empty())
            return;
        QList<RcloneFilePtr> lst;
        for (auto item: m_current_file_list)
            lst << item->getFile();
        m_treeFileView1->treeFileView()->copyto(m_current_file_list);
    });
    connect(m_treeFileView2->treeFileView(), &TreeFileView::pasted, this, [this](const RcloneFilePtr &file)
    {
        if (m_current_file_list.empty())
            return;
        QList<RcloneFilePtr> lst;
        for (auto item: m_current_file_list)
            lst << item->getFile();
        m_treeFileView2->treeFileView()->copyto(m_current_file_list);
    });

    connect(m_treeFileView1->treeFileView(), &TreeFileView::taskAdded, this,
            [this](const QString &src, const QString &dst, const RclonePtr &rclone,
                   const std::function<void()> &callable, const Rclone::TaskType &type)
            {
                emit taskAdded(src, dst, rclone, callable, type);
            });

    connect(m_treeFileView2->treeFileView(), &TreeFileView::taskAdded, this,
            [this](const QString &src, const QString &dst, const RclonePtr &rclone,
                   const std::function<void()> &callable, const Rclone::TaskType &type)
            {
                emit taskAdded(src, dst, rclone, callable, type);
            });
}

void FileViewWidget::changeRemote(const std::shared_ptr<remotes_selected> &remotes)
{
    m_current_file_list.clear();

    if (remotes->first not_eq nullptr)
    {
        if (remotes->first->remoteInfo().get() != m_treeFileView1->treeFileView()->remoteInfo().get())
        {
            m_treeFileView1->treeFileView()->changeRemote(remotes->first->remoteInfo());
        }
    } else
        m_treeFileView1->treeFileView()->changeRemote(nullptr);


    if (remotes->second not_eq nullptr)
    {
        if (remotes->second->remoteInfo().get() != m_treeFileView2->treeFileView()->remoteInfo().get())
        {
            m_treeFileView2->treeFileView()->changeRemote(remotes->second->remoteInfo());
        }
    } else
        m_treeFileView2->treeFileView()->changeRemote(nullptr);
}