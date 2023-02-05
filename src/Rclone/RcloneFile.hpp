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
#include "../Remote/Remote.h"

class RcloneFile : public QObject
{
Q_OBJECT

	QString path{};
	uint64_t size{};
	bool isDirectory{};
	QDateTime modTime{};
	Remote typeFile{};

	void init(const QString &path, Remote type = Remote::Local);

public:
	explicit RcloneFile(const QFile &file, Remote typeFile = Remote::Local);

	RcloneFile(const QString &pathFile, Remote typeFile = Remote::Local);

	explicit RcloneFile(const QDir &dir, Remote typeFile = Remote::Local);

	RcloneFile(const QString &path, uint64_t size, bool isDir, QDateTime modTime,
			   Remote typeFile = Remote::Local);

	[[nodiscard]] const QString &getPath() const;

	void setPath(const QString &path);

	[[nodiscard]] uint64_t getSize() const;

	void setSize(uint64_t size);

	[[nodiscard]] bool isDir() const;

	void setIsDir(bool isDir);

	[[nodiscard]] const QDateTime &getModTime() const;

	void setModTime(const QDateTime &modTime);

	[[nodiscard]] QString getName() const;

	[[nodiscard]] QString getSizeString() const;

	[[nodiscard]] QString getPathString() const;

	[[nodiscard]] QString getModTimeString() const;

	[[nodiscard]] QString getIsDirString() const;


};


#endif //IRIDIUM_RCLONEFILE_HPP
