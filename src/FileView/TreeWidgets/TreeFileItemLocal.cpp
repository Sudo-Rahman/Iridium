//
// Created by Rahman on 31/03/2023.
//

#include "TreeFileItemLocal.hpp"

TreeFileItemLocal::TreeFileItemLocal(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent)
        : TreeFileItem(parent)
{
    TreeFileItem::m_file = std::make_shared<RcloneFile>(
            path,
            remoteInfo
    );
    setText(m_file->getName());
    QStandardItem::setData(m_file->getName(), SORT_ROLE);
    initIcon();
}
