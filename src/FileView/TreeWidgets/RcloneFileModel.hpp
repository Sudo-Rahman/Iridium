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
    RemoteInfoPtr m_remote_info{};
    QModelIndex m_root_index{};

    QTreeView *m_view{};

    bool m_expand_or_double_click = false;

    bool m_check_is_valid = false;

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
        m_expand_or_double_click = expandOrDoubleClick;
    }

};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
