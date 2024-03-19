//
// Created by sr-71 on 07/05/2023.
//

#include "SearchTableItem.hpp"
#include <Utility/Utility.hpp>

SearchTableItem::SearchTableItem(const int &col, const RcloneFilePtr &file)
{
    _file = file;
    switch (col)
    {
        case 0:
            setText(file->getName());
            SearchTableItem::setData(file->getName(), Qt::UserRole + 1);
//             use lot of memory
            setIcon(file->getIcon().pixmap(16, 16));
            break;
        case 1:
            setText(file->getRemoteInfo()->name().c_str());
            SearchTableItem::setData(file->getRemoteInfo()->name().c_str(), Qt::UserRole + 1);
            setIcon(QIcon(file->getRemoteInfo()->icon().c_str()).pixmap(16, 16));
            break;
        case 2:
            setText(file->getPath());
            SearchTableItem::setData(file->getPath(), Qt::UserRole + 1);
            break;
        case 3:
            setText(Iridium::Utility::sizeToString(file->getSize()).c_str());
            SearchTableItem::setData((qulonglong) file->getSize(), Qt::UserRole + 1);
            break;
        case 4:
            setText(file->getModTimeString());
            SearchTableItem::setData(file->getModTime(), Qt::UserRole + 1);
            break;
        case 5:
            setText(file->getFileType());
            SearchTableItem::setData(file->getFileType(), Qt::UserRole + 1);
            break;
        default:
            break;
    }
}
