//
// Created by sr-71 on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <utility>

void RcloneFile::init(const QString &path, TypeFile type)
{
    QFileInfo info(path);
    if (info.exists() and type == TypeFile::Local)
    {
        setSize(info.size());
        setModTime(info.lastModified());
    }
    if (type == TypeFile::Local)
        setPath(info.absoluteFilePath());
    else
        setPath(path);
    setIsDir(info.isDir());
}


RcloneFile::RcloneFile(const QFile &file, TypeFile type)
{
    init(QFileInfo(file).filePath(), type);
}

RcloneFile::RcloneFile(const QString &path, TypeFile type)
{
    init(path, type);
}

RcloneFile::RcloneFile(const QDir &dir, TypeFile type)
{
    init(dir.path(), type);
}

RcloneFile::RcloneFile(const QString &path, uint64_t size, bool isDir, QDateTime modTime, TypeFile type)
        : size(size), isDirectory(isDir), modTime(std::move(modTime)), typeFile(type)
{
    setPath(path);
}


const QString &RcloneFile::getPath() const
{
    return path;
}

void RcloneFile::setPath(const QString &path)
{
    RcloneFile::path = path;
    if (not path.endsWith("/") and not path.isEmpty() and isDirectory)
        RcloneFile::path += "/";
}

uint64_t RcloneFile::getSize() const
{
    return size;
}

void RcloneFile::setSize(uint64_t size)
{
    RcloneFile::size = size;
}

const QDateTime &RcloneFile::getModTime() const
{
    return modTime;
}

void RcloneFile::setModTime(const QDateTime &modTime)
{
    RcloneFile::modTime = modTime;
}

QString RcloneFile::getName() const
{
    return QFileInfo(path).fileName();
}

bool RcloneFile::isDir() const
{
    return isDirectory;
}

void RcloneFile::setIsDir(bool isDir)
{
    RcloneFile::isDirectory = isDir;
}


