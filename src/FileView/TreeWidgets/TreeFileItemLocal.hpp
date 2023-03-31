//
// Created by sr-71 on 31/03/2023.
//

#ifndef IRIDIUM_TREEFILEITEMLOCAL_HPP
#define IRIDIUM_TREEFILEITEMLOCAL_HPP

#include "TreeFileItem.hpp"

class TreeFileItemLocal : public TreeFileItem
{
public:
	explicit TreeFileItemLocal(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem * = nullptr);


};


#endif //IRIDIUM_TREEFILEITEMLOCAL_HPP
