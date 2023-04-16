//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <Settings.hpp>
#include <QMimeDatabase>


const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
	return m_file;
}

TreeFileItem::TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem *parent) : m_parent(
	parent), QStandardItem()
{
	if (parent not_eq nullptr)
		parent->addChild(this);
	TreeFileItem::m_file = std::make_shared<RcloneFile>(
		path,
		remoteInfo
	);
	setText(path);
	initIcon();
}

TreeFileItem::TreeFileItem(const QString &path, const std::shared_ptr<RcloneFile> &file, TreeFileItem *parent,
						   bool initIco) : m_parent(parent)
{
	if (parent not_eq nullptr)
		parent->addChild(this);
	TreeFileItem::m_file = file;
	setText(path);
	if (initIco)
		initIcon();
}

TreeFileItem *TreeFileItem::getParent() const
{
	return m_parent;
}

void TreeFileItem::initIcon()
{
	QIcon ico;
	if (m_file->isDir())
	{
		if (QFileInfo(m_file->getName()).suffix() == "app")
			ico = QIcon::fromTheme(m_file->getName().toLower().remove(".app").replace(" ", "-"),
								   QIcon::fromTheme("application-default-icon"));
		else ico = Settings::DIR_ICON;
		m_file->setSize(0);
	} else
	{
		if (QFileInfo(m_file->getName()).suffix() == "exe")
			ico = QIcon::fromTheme(m_file->getName().toLower().remove(".exe").replace(" ", "-"));
		if (ico.isNull())
			ico = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(m_file->getPath()).iconName(),
								   QIcon::fromTheme(
									   QMimeDatabase().mimeTypeForFile(m_file->getPath()).genericIconName()));
		if (ico.isNull())
			ico = QIcon::fromTheme("unknown");
	}
	setIcon(ico);
}

TreeFileItem::TreeFileItem(TreeFileItem *parent) : m_parent(parent)
{
	if (parent not_eq nullptr)
		parent->addChild(this);
}
