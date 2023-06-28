//
// Created by Rahman on 02/02/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODEL_HPP
#define IRIDIUM_RCLONEFILEMODEL_HPP

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include <QTreeView>
#include <Rclone.hpp>
#include <QMimeData>

class TreeMimeData : public QMimeData
{
Q_OBJECT

    QList<TreeFileItem *> _items{};
public:
    explicit TreeMimeData(const QList<TreeFileItem *> &items) : _items(items) {}

    [[nodiscard]] QList<TreeFileItem *> items() const { return _items; }

    [[nodiscard]] bool hasFormat(const QString &mimeType) const override
    {
        return mimeType == "application/x-qabstractitemmodeldatalist";
    }
};

class RcloneFileModel : public QStandardItemModel
{
Q_OBJECT

    RcloneUniquePtr _rclone{};

public:
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    QStringList mimeTypes() const override;

    bool
    dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;


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

    virtual void reload(TreeFileItem *item = nullptr) = 0;

    [[nodiscard]] const QModelIndex &getRootIndex() const;

    static QList<QStandardItem *> getItemList(TreeFileItem *item);

    void setExpandOrDoubleClick(bool expandOrDoubleClick)
    {
        _expand_or_double_click = expandOrDoubleClick;
    }

    bool fileInFolder(const RcloneFilePtr &, TreeFileItem *folder);

    QList<RcloneFilePtr> filesInFolder( TreeFileItem *folder);

    TreeFileItem *getTreeFileItem(const RcloneFilePtr &file, TreeFileItem *parent);

    virtual void stop() = 0;
};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
