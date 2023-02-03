//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <QJsonObject>
#include <QDateTime>
#include <QObject>

TreeFileItem::TreeFileItem(QString path, QJsonObject data)
{
	if (path.contains(":"))
		file = std::make_shared<RcloneFile>(
			path + data["Path"].toString(),
			static_cast<uint64_t>(data["Size"].toInteger()),
			data["IsDir"].toBool(),
			QDateTime::fromString(data["ModTime"].toString(), Qt::ISODateWithMs),
			RcloneFile::Distant);
	else
		file = std::make_shared<RcloneFile>(
			path + data["Name"].toString(),
			RcloneFile::Local);
	setText(file->isDir() ? file->getPathString() : file->getName());
}

const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
	return file;
}

TreeFileItem::TreeFileItem(QString path): QStandardItem()
{
	setText(path);
}

TreeFileItem::TreeFileItem(const RcloneFile &file)
{
	TreeFileItem::file = std::make_shared<RcloneFile>(
		file.getPath(),
		file.getSize(),
		file.isDir(),
		file.getModTime()
		);
	setText(file.getName());
}

