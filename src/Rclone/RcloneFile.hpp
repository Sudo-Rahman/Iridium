//
// Created by sr-71 on 10/01/2023.
//

#ifndef IRIDIUM_RCLONEFILE_HPP
#define IRIDIUM_RCLONEFILE_HPP


#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <Remote.h>

class RcloneFile : public QObject
{
Q_OBJECT

	RemoteInfoPtr m_remoteInfo{};
	QString path{};
	uint64_t size{};
	uint32_t objs{};
	bool isDirectory{};
	QDateTime modTime{};

	void init();

public:

	RcloneFile(const QString &pathFile, const RemoteInfoPtr &remoteInfo);

	RcloneFile(const QString &pathFile, uint64_t size, bool isDir, QDateTime modTime,
			   const RemoteInfoPtr &remoteInfo);

	[[nodiscard]] const QString &getPath() const;

	void setPath(const QString &path);

	[[nodiscard]] uint64_t getSize() const;

	void setSize(uint64_t size);

	[[nodiscard]] bool isDir() const;

	void setIsDir(bool isDir);

	[[nodiscard]] const QDateTime &getModTime() const;

	void setModTime(const QDateTime &modTime);

	[[nodiscard]] QString getName() const;

	void changeName(const QString &newName);

	[[nodiscard]] QString getSizeString() const;

	[[nodiscard]] QString getPathString() const;

	[[nodiscard]] QString getModTimeString() const;

	[[nodiscard]] QString getIsDirString() const;

	[[nodiscard]] std::shared_ptr<RemoteInfo> getRemoteInfo() const;

	[[nodiscard]] uint32_t getObjs() const;

	void setObjs(uint32_t objs);

	[[nodiscard]] QString getFileType() const;

	// define copy operator and copy constructor
	RcloneFile(const RcloneFile &other)
	{
		path = other.path;
		size = other.size;
		isDirectory = other.isDirectory;
		modTime = other.modTime;
		m_remoteInfo = other.m_remoteInfo;
	}
};

typedef std::shared_ptr<RcloneFile> RcloneFilePtr;


#endif //IRIDIUM_RCLONEFILE_HPP
