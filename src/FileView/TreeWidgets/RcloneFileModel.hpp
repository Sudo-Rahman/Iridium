//
// Created by Rahman on 02/02/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODEL_HPP
#define IRIDIUM_RCLONEFILEMODEL_HPP

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include <QTreeView>
#include <Rclone.hpp>


class RcloneFileModel : public QStandardItemModel
{
Q_OBJECT

protected:
    RemoteInfoPtr _remote_info{};
    QModelIndex _root_index{};

    QTreeView *_view{};

    bool _expand_or_double_click = false;

    bool _check_is_valid = false;

    virtual void init() = 0;

    explicit RcloneFileModel();

    explicit RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View);

    void addProgressBar(const QModelIndex &index);

public:
    virtual void addItem(const RcloneFilePtr &file, TreeFileItem *parent);

    [[nodiscard]] const QModelIndex &getRootIndex() const;

    static QList<QStandardItem *> getItemList(TreeFileItem *item);

    void setExpandOrDoubleClick(bool expandOrDoubleClick)
    {
        _expand_or_double_click = expandOrDoubleClick;
    }

    virtual void stop() = 0;
};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
