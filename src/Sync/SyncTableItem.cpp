//
// Created by sr-71 on 16/06/2023.
//

#include "SyncTableItem.hpp"

SyncTableItem::SyncTableItem(const RcloneFilePtr &file, int col)
{
    _file = file;
    setEditable(false);
    switch (col)
    {
        case 0:{
            setText(file->getName());
        }
            break;
        case 1:{
            setText(QObject::tr("Oui"));
        }
            break;
        case 2:
            break;
        case 3:{
        }

    }
}