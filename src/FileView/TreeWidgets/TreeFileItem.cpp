//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <QJsonObject>
#include <QDateTime>
#include <QMimeDatabase>

TreeFileItem::TreeFileItem(QString path, QJsonObject data, TreeFileItem *parent) : parent(parent)
{
	setSizeHint(QSize(35, 40));
	if (path.contains(":"))
		file = std::make_shared<RcloneFile>(
			path + data["Path"].toString(),
			static_cast<uint64_t>(data["Size"].toInteger()),
			data["IsDir"].toBool(),
			QDateTime::fromString(data["ModTime"].toString(), Qt::ISODateWithMs),
			Remote::Distant);
	else
		file = std::make_shared<RcloneFile>(
			path + data["Name"].toString(),
			Remote::Local);
	setText(file->getName());
	QIcon ico;
	if (file->isDir())
	{
		ico = QIcon::fromTheme("default-folder");
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

const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
	return file;
}

TreeFileItem::TreeFileItem(QString path, const std::shared_ptr<RcloneFile> &file, TreeFileItem *parent) : parent(
	parent), QStandardItem()
{
	setSizeHint(QSize(35, 40));
	setText(path);
	if (file == nullptr)
		TreeFileItem::file = std::make_shared<RcloneFile>(
			path
		);
	else
		TreeFileItem::file = file;

}

TreeFileItem::TreeFileItem(const RcloneFile &file, TreeFileItem *parent) : parent(parent)
{
	setSizeHint(QSize(35, 40));
	TreeFileItem::file = std::make_shared<RcloneFile>(
		file.getPath(),
		file.getSize(),
		file.isDir(),
		file.getModTime()
	);
	setText(file.getName());
}

TreeFileItem *TreeFileItem::getParent() const
{
	return parent;
}
