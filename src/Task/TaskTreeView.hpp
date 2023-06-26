//
// Created by Rahman on 12/04/2023.
//

#ifndef IRIDIUM_TASKTREEVIEW_HPP
#define IRIDIUM_TASKTREEVIEW_HPP

#include <QList>
#include <QTreeView>
#include <QLayout>
#include <QThread>
#include <QModelIndex>
#include "TaskRow.hpp"

struct Tasks
{
    TaskRowPtr parent;
    RclonePtr rclone;
    std::map<size_t, TaskRowPtr> children;
};

class TaskTreeView : public QTreeView
{
Q_OBJECT

    static std::unordered_map<size_t, Tasks> _tasks;

    QStandardItemModel *_model{};

public:
    explicit TaskTreeView(QWidget *parent = nullptr);

    void addTask(const QString &src, const QString &dst, const RclonePtr &rclone, const std::function<void()> &callable,
                 const Rclone::TaskType &type = Rclone::Unknown);

signals:

    void taskFinished(std::pair<size_t, Tasks>);

};


#endif //IRIDIUM_TASKTREEVIEW_HPP
