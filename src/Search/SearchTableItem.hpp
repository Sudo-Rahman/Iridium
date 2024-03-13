//
// Created by sr-71 on 07/05/2023.
//

#ifndef IRIDIUM_SEARCHTABLEITEM_HPP
#define IRIDIUM_SEARCHTABLEITEM_HPP

#include <QStandardItem>
#include <RcloneFile.hpp>

class SearchTableItem : public QStandardItem
{
    RcloneFilePtr _file;
public:
    explicit SearchTableItem(const int &col, const RcloneFilePtr &file);

    [[nodiscard]] const RcloneFilePtr &getFile() const { return _file; }
};


#endif //IRIDIUM_SEARCHTABLEITEM_HPP
