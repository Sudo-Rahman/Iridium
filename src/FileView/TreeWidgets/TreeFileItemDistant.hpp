//
// Created by sr-71 on 31/03/2023.
//

#ifndef IRIDIUM_TREEFILEITEMDISTANT_HPP
#define IRIDIUM_TREEFILEITEMDISTANT_HPP

#include "TreeFileItem.hpp"
#include <QJsonObject>
#include <QDateTime>

class TreeFileItemDistant : public TreeFileItem
{
public:
	explicit TreeFileItemDistant(const QString &path, const RemoteInfoPtr &remoteInfo, const QJsonObject &data,
								 TreeFileItem * = nullptr);

};


#endif //IRIDIUM_TREEFILEITEMDISTANT_HPP
