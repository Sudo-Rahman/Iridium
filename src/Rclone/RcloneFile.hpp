//
// Created by Rahman on 10/01/2023.
//

#ifndef IRIDIUM_RCLONEFILE_HPP
#define IRIDIUM_RCLONEFILE_HPP


#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QJsonDocument>
#include <Remote.h>

class RcloneFile : public QObject
{
Q_OBJECT

    RemoteInfoPtr m_remote_info{};
    QString m_path{};
    uint64_t m_size{};
    uint32_t m_objs{};
    bool m_is_dir{};
    QDateTime m_mod_time{};

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

    [[nodiscard]] std::shared_ptr<RemoteInfo> getRemoteInfo() const;

    [[nodiscard]] uint32_t getObjs() const;

    void setObjs(uint32_t objs);

    [[nodiscard]] QString getFileType() const;

    [[nodiscard]] QIcon getIcon();

    // define copy operator and copy constructor
    RcloneFile(const RcloneFile &other)
    {
        setPath(other.m_path);
        m_size = other.m_size;
        m_is_dir = other.m_is_dir;
        m_mod_time = other.m_mod_time;
        m_remote_info = other.m_remote_info;
    }

    // ostream operator
    friend std::ostream &operator<<(std::ostream &os, const RcloneFile &file)
    {
        os << "path: " << file.m_path.toStdString() << std::endl;
        os << "size: " << file.m_size << std::endl;
        os << "m_is_dir: " << file.m_is_dir << std::endl;
        os << "modTime: " << file.m_mod_time.toString().toStdString() << std::endl;
        os << "remoteInfo: " << file.m_remote_info->name() << std::endl;
        return os;
    }
};

typedef std::shared_ptr<RcloneFile> RcloneFilePtr;


#endif //IRIDIUM_RCLONEFILE_HPP
