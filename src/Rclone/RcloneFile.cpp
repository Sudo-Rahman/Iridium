//
// Created by sr-71 on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <cmath>

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
	QString name;
	path.contains(":") ? name = QFileInfo(path.split(":")[1]).fileName() : name = QFileInfo(path).fileName();
	return name;
}

bool RcloneFile::isDir() const
{
	return isDirectory;
}

void RcloneFile::setIsDir(bool isDir)
{
	RcloneFile::isDirectory = isDir;
}

QString RcloneFile::getSizeString() const
{
	if (isDirectory)
		return "--";
	if (size / pow(1024, 4.0) > 1)
		return QString::number(int(size / pow(1024.0, 4.0) * 100) / 100.0) + " TiB";

	if (size / pow(1024, 3.0) > 1)
		return QString::number(int(size / pow(1024.0, 3.0) * 100) / 100.0) + " GiB";

	if (size / pow(1024, 2.0) > 1)
		return QString::number(int(size / pow(1024.0, 2.0) * 100) / 100.0) + " MiB";

	if (size / 1024.0 > 1)
		return QString::number(int(size / pow(1024.0, 1.0) * 100) / 100.0) + " KiB";


	return QString::number(size) + " B";
}

QString RcloneFile::getPathString() const
{
	QString tmpPath;
	if (isDirectory and typeFile == Distant and path.contains(":"))
	{
		tmpPath = path.split(":")[1];
	}
	return QDir(tmpPath).dirName();
}

QString RcloneFile::getModTimeString() const
{
	return modTime.toString(tr("dd MMM yyyy à hh:mm:ss"));
}

QString RcloneFile::getIsDirString() const
{
	if (isDirectory)
		return "Dossier";
	else
		return "Fichier";
}


