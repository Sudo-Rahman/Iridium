//
// Created by sr-71 on 18/01/2023.
//

#include "TreeFileItem.hpp"
#include <QJsonObject>
#include <QDateTime>
#include <QObject>

TreeFileItem::TreeFileItem(const QString &path, const QJsonObject &data, QTreeWidgetItem *parent)
        : QTreeWidgetItem(parent)
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

    setText(0, file->isDir() ? file->getPathString() : file->getName());
    setText(1, file->getSizeString());
    setText(2, file->getModTimeString());
    setText(3, file->isDir() ? QObject::tr("Dossier") : QObject::tr("Ficher"));
}

const std::shared_ptr<RcloneFile> &TreeFileItem::getFile() const
{
    return file;
}
