//
// Created by sr-71 on 16/06/2023.
//

#ifndef IRIDIUM_SYNCTABLEITEM_HPP
#define IRIDIUM_SYNCTABLEITEM_HPP


#include <QStandardItem>
#include <RcloneFile.hpp>

class SyncTableItem : public QStandardItem
{
    RcloneFilePtr _file;
public:
    explicit SyncTableItem(const RcloneFilePtr &file, int col);

    [[nodiscard]] const RcloneFilePtr &getFile() const { return _file; }
};


#endif //IRIDIUM_SYNCTABLEITEM_HPP
