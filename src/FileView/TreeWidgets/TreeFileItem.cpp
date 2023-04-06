//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include "../../Config/Settings.hpp"
#include <QMimeDatabase>


const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
	return file;
}

TreeFileItem::TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent) : parent(
	parent), QStandardItem()
{
	if (parent not_eq nullptr)
		parent->addChild(this);
	TreeFileItem::file = std::make_shared<RcloneFile>(
		path,
		remoteInfo
	);
	setText(path);
	initIcon();
}

TreeFileItem::TreeFileItem(const QString &path, const std::shared_ptr<RcloneFile> &file, TreeFileItem *parent,
						   bool initIco) : parent(parent)
{
	if (parent not_eq nullptr)
		parent->addChild(this);
	TreeFileItem::file = file;
	setText(path);
	if (initIco)
		initIcon();
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
		ico = Settings::DIR_ICON;
		file->setSize(0);
	} else
	{
		ico = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(file->getPath()).iconName(),
							   QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(file->getPath()).genericIconName()));
		if (ico.isNull())
			ico = QIcon::fromTheme("unknown");
	}
	setIcon(ico);
}

TreeFileItem::TreeFileItem(TreeFileItem *parent) : parent(parent)
{
	if (parent not_eq nullptr)
		parent->addChild(this);
}
