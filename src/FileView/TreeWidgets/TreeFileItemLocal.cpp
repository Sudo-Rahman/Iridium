//
// Created by sr-71 on 31/03/2023.
//

#include "TreeFileItemLocal.hpp"

TreeFileItemLocal::TreeFileItemLocal(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent)
	: TreeFileItem(parent)
{
	TreeFileItem::file = std::make_shared<RcloneFile>(
		path,
		remoteInfo
	);
	setText(file->getName());
	initIcon();
}
