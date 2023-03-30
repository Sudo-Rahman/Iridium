//
// Created by sr-71 on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <cmath>
#include "../Utility/Utility.hpp"

void RcloneFile::init(const QString &path, Remote type)
{
	QFileInfo info(path);
	if (info.exists() and type == Remote::Local)
	{
		setSize(info.size());
		setModTime(info.lastModified());
		setPath(info.absoluteFilePath());

	} else
	{
		setPath(path);
		RcloneFile::typeFile = Distant;
	}
	setIsDir(info.isDir());
}


RcloneFile::RcloneFile(const QFile &file, Remote type)
{
	init(QFileInfo(file).filePath(), type);
}

RcloneFile::RcloneFile(const QString &path, Remote type)
{
	init(path, type);
}

RcloneFile::RcloneFile(const QDir &dir, Remote type)
{
	init(dir.path(), type);
}

RcloneFile::RcloneFile(const QString &path, uint64_t size, bool isDir, QDateTime modTime, Remote type)
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
	QString name;
	if (isDirectory)
		return getPathString();
	path.contains(":") ? name = QFileInfo(path.split(":")[1]).fileName() : name = QFileInfo(path).fileName();
	return name;
}

bool RcloneFile::isDir() const
{
	return isDirectory;
}

void RcloneFile::setIsDir(bool isDir)
{
	if (path.endsWith(":"))
		RcloneFile::isDirectory = true;
	else
		RcloneFile::isDirectory = isDir;
}

QString RcloneFile::getSizeString() const
{
	if (size == 0)
		return "--";
	return QString::fromStdString(Utility::sizeToString(size));
}

QString RcloneFile::getPathString() const
{
	QString tmpPath = path;
	if (isDirectory and typeFile == Distant and path.contains(":"))
	{
		tmpPath = path.split(":")[1];
	}
	return QDir(tmpPath).dirName();
}

QString RcloneFile::getModTimeString() const
{
	return QLocale::system().toString(modTime, tr("dd MMM yyyy à hh:mm:ss"));
}

QString RcloneFile::getIsDirString() const
{
	if (isDirectory)
		return tr("Dossier");
	else
		return tr("Fichier");
}


Remote RcloneFile::getTypeFile() const
{
	return typeFile;
}

uint32_t RcloneFile::getObjs() const
{
	return objs;
}

void RcloneFile::setObjs(uint32_t objs)
{
	RcloneFile::objs = objs;
}

