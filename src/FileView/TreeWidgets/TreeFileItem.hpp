//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEITEM_HPP
#define IRIDIUM_TREEFILEITEM_HPP

#include <QTreeWidgetItem>
#include "../../Rclone/RcloneFile.hpp"

class TreeFileItem : public QTreeWidgetItem
{
    std::shared_ptr<RcloneFile> file{};

public:
    explicit TreeFileItem(const QString &path, const QJsonObject &data, QTreeWidgetItem *parent = nullptr);

    const std::shared_ptr<RcloneFile> &getFile() const;

};


#endif //IRIDIUM_TREEFILEITEM_HPP
