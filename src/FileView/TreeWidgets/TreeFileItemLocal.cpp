//
// Created by Rahman on 31/03/2023.
//

#include "TreeFileItemLocal.hpp"

TreeFileItemLocal::TreeFileItemLocal(const RcloneFile &file, TreeFileItem *parent)
{
    TreeFileItem::_file = std::make_shared<RcloneFile>(file);
    setText(_file->getName());
    QStandardItem::setData(_file->getName(), SORT_ROLE);
    initIcon();
    if (_file->isDir())
        appendRow(TreeFileItem::decorateList());
    else
        setFlags(flags() & ~Qt::ItemIsDropEnabled);
}
