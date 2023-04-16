//
// Created by sr-71 on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <QMimeDatabase>
#include <Utility/Utility.hpp>

void RcloneFile::init()
{
	QFileInfo info(path);
	if (m_remoteInfo->isLocal())
	{
		setSize(info.size());
		setModTime(info.lastModified());
		setPath(info.absoluteFilePath());

	}
	setIsDir(info.isDir());
	if (m_remoteInfo->isLocal() and isDirectory)
		setPath(path);
}


RcloneFile::RcloneFile(const QString &pathFile, const RemoteInfoPtr &remoteInfo)
{
	path = pathFile;
	m_remoteInfo = remoteInfo;
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
	return QString::fromStdString(Iridium::Utility::sizeToString(size));
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

QString RcloneFile::getFileType() const
{
	if (isDirectory)
		return tr("Dossier");
	auto mime = QMimeDatabase().mimeTypeForFile(getPath());
	if (mime.name() != "application/octet-stream")
	{
		auto type = mime.comment();
		type.front() = type.front().toUpper();
		return type;
	} else
		return tr("Document ") + QFileInfo(getPath()).suffix().toUpper();
}

/**
 * @brief Change the name of the file
 * @param newName
 */
void RcloneFile::changeName(const QString &newName)
{
	if (not m_remoteInfo->isLocal() and not path.contains("/"))
	{
		path = path.split(":")[0] + ":" + newName;
		return;
	}
	if (isDirectory and path.endsWith("/"))
		path.remove(path.size() - 1, 1);
	if (path.contains("/"))
		path = path.remove(path.lastIndexOf("/") + 1, path.split("/").last().size());
	else if (not m_remoteInfo->isLocal())
		path = path.remove(path.lastIndexOf(":") + 1, path.split(":").last().size());
	path.append(newName);
	if (isDirectory)
		path.append("/");
}

