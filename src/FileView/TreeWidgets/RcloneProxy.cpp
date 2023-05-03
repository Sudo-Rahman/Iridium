//
// Created by Rahman on 08/03/2023.
//

#include "RcloneProxy.hpp"
#include <RcloneFileModel.hpp>
#include <TreeFileItem.hpp>

RcloneProxy::RcloneProxy(QObject *parent) : QSortFilterProxyModel(parent)
{


}

bool RcloneProxy::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    qDebug() << source_left << source_right;
    if (not source_left.isValid() or not source_right.isValid())
        return false;

    auto model = dynamic_cast<RcloneFileModel *>(sourceModel());
    auto item_left = dynamic_cast<TreeFileItem *>(model->itemFromIndex(source_left));
    auto item_right = dynamic_cast<TreeFileItem *>(model->itemFromIndex(source_right));
    if (item_left not_eq nullptr and item_right not_eq nullptr)
    {
        auto column = source_left.column() == source_right.column() ? source_left.column() : -1;
        // get column
        switch (source_left.column())
        {
            case 0:
                return item_left->getFile()->getName() < item_right->getFile()->getName();
            case 1:
                return item_left->getFile()->getSize() < item_right->getFile()->getSize();
            case 2:
                return item_left->getFile()->getModTime() < item_right->getFile()->getModTime();
            case 3:
                return item_left->getFile()->getFileType() < item_right->getFile()->getFileType();
            default:
                return false;
        }
    }
    return true;
}