//
// Created by sr-71 on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <cmath>
#include "../Utility/Utility.hpp"

void RcloneFile::init()
{
	QFileInfo info(path);
	if (m_remoteInfo->isLocal())
	{
		setSize(info.size());
		setModTime(info.lastModified());
		setPath(info.absoluteFilePath());

	}
	setPath(path);
	setIsDir(info.isDir());
}


RcloneFile::RcloneFile(const QString &pathFile, const RemoteInfoPtr &remoteInfo)
{
	m_remoteInfo = remoteInfo;
	setPath(pathFile);
	init();
}

RcloneFile::RcloneFile(const QString &pathFile, uint64_t size, bool isDir, QDateTime modTime,
					   const RemoteInfoPtr &remoteInfo)
	: size(size), isDirectory(isDir), modTime(std::move(modTime))
{
	m_remoteInfo = remoteInfo;
	setPath(pathFile);
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
	if (isDirectory and !m_remoteInfo->isLocal() and path.contains(":"))
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


uint32_t RcloneFile::getObjs() const
{
	return objs;
}

void RcloneFile::setObjs(uint32_t objs)
{
	RcloneFile::objs = objs;
}

std::shared_ptr<RemoteInfo> RcloneFile::getRemoteInfo() const
{
	return m_remoteInfo;
}

