//
// Created by sr-71 on 31/03/2023.
//

#include "TreeFileItemDistant.hpp"

TreeFileItemDistant::TreeFileItemDistant(const QString &path, const RemoteInfoPtr &remoteInfo, const QJsonObject &data,
										 TreeFileItem *parent) : TreeFileItem(parent)
{
	m_file = std::make_shared<RcloneFile>(
		path + data["Name"].toString(),
		static_cast<uint64_t>(data["Size"].toInteger()),
		data["IsDir"].toBool(),
		QDateTime::fromString(data["ModTime"].toString(), Qt::ISODateWithMs),
		remoteInfo);
	setText(m_file->getName());
	initIcon();
}
