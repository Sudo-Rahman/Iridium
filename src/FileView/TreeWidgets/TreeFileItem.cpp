//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <QMimeDatabase>


const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
	return file;
}

TreeFileItem::TreeFileItem(const QString& path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent) : parent(
	parent), QStandardItem()
{
	TreeFileItem::file = std::make_shared<RcloneFile>(
		path,
		remoteInfo
	);
	setText(path);
	initIcon();
}

TreeFileItem::TreeFileItem(const QString& path, const std::shared_ptr<RcloneFile> &file, TreeFileItem *parent) : parent(parent)
{
	TreeFileItem::file = file;
	setText(path);
}

TreeFileItem *TreeFileItem::getParent() const
{
	return parent;
}

void TreeFileItem::initIcon()
{
	QIcon ico;
	if (file->isDir())
	{
		ico = QIcon::fromTheme("yellow-folder");
		file->setSize(0);
	} else
	{
		ico = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(file->getPath()).iconName());
		if (ico.isNull())
			ico = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(file->getPath()).genericIconName(),
								   QIcon::fromTheme("unknown"));
	}
	setIcon(ico);
}

TreeFileItem::TreeFileItem(TreeFileItem *parent) : parent(parent)
{}
