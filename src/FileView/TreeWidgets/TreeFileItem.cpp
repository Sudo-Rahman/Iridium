//
// Created by Rahman on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <Settings.hpp>
#include <QMimeDatabase>


const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
    return _file;
}

TreeFileItem::TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent) : _parent(
        parent), QStandardItem()
{
    if (parent not_eq nullptr)
        parent->addChild(this);
    TreeFileItem::_file = std::make_shared<RcloneFile>(
            path,
            remoteInfo
    );
    setText(path);
    QStandardItem::setData(path, SORT_ROLE);
    setFlags(flags() & ~Qt::ItemIsDropEnabled);
}

TreeFileItem::TreeFileItem(const int &column, const std::shared_ptr<RcloneFile> &file, TreeFileItem *parent) : _parent(
        parent)
{
    if (parent not_eq nullptr)
        parent->addChild(this);
    TreeFileItem::_file = file;
    switch (column)
    {
        case 0:
            setText(file->getName());
            QStandardItem::setData(file->getName(), SORT_ROLE);
            initIcon();
            break;
        case 1:
            setText(file->getSizeString());
            QStandardItem::setData((qulonglong) file->getSize(), SORT_ROLE);
            break;
        case 2:
            setText(file->getModTimeString());
            QStandardItem::setData(file->getModTime(), SORT_ROLE);
            break;
        case 3:
            setText(file->getFileType());
            QStandardItem::setData(file->getFileType(), SORT_ROLE);
            break;
        default:
            break;
    }
    if (not _file->isDir())
        setFlags(flags() & ~Qt::ItemIsDropEnabled);
}

TreeFileItem *TreeFileItem::getParent() const
{
    return _parent;
}

void TreeFileItem::initIcon()
{
    setIcon(_file->getIcon().pixmap(32, 32));
    setToolTip(_file->getName());
}

TreeFileItem::TreeFileItem(TreeFileItem *parent) : _parent(parent)
{
    if (parent not_eq nullptr)
        parent->addChild(this);
}