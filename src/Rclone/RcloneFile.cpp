//
// Created by Rahman on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <QMimeDatabase>
#include <Settings.hpp>
#include <Utility/Utility.hpp>

void RcloneFile::init()
{
    QFileInfo info(m_path);
    if (m_remote_info->isLocal())
    {
        setSize(info.size());
        setModTime(info.lastModified());
        setPath(info.absoluteFilePath());

    }
    setIsDir(info.isDir());
    if (m_remote_info->isLocal() and m_is_dir)
        setPath(m_path);
}


RcloneFile::RcloneFile(const QString &pathFile, const RemoteInfoPtr &remoteInfo)
{
    m_path = pathFile;
    m_remote_info = remoteInfo;
    init();
}

RcloneFile::RcloneFile(const QString &pathFile, uint64_t size, bool isDir, QDateTime modTime,
                       const RemoteInfoPtr &remoteInfo)
        : m_size(size), m_is_dir(isDir), m_mod_time(std::move(modTime))
{
    m_remote_info = remoteInfo;
    setPath(pathFile);
}


const QString &RcloneFile::getPath() const
{
    return m_path;
}

void RcloneFile::setPath(const QString &path)
{
    RcloneFile::m_path = path;
    if (not path.endsWith("/") and not path.isEmpty() and m_is_dir)
        RcloneFile::m_path += "/";
}

uint64_t RcloneFile::getSize() const
{
    return m_size;
}

void RcloneFile::setSize(uint64_t size)
{
    RcloneFile::m_size = size;
}

const QDateTime &RcloneFile::getModTime() const
{
    return m_mod_time;
}

void RcloneFile::setModTime(const QDateTime &modTime)
{
    RcloneFile::m_mod_time = modTime;
}

QString RcloneFile::getName() const
{
    QString name;
    if (m_is_dir)
        return getPathString();
    m_path.contains(":") ? name = QFileInfo(m_path.split(":")[1]).fileName() : name = QFileInfo(m_path).fileName();
    return name;
}

bool RcloneFile::isDir() const
{
    return m_is_dir;
}

void RcloneFile::setIsDir(bool isDir)
{
    if (m_path.endsWith(":"))
        RcloneFile::m_is_dir = true;
    else
        RcloneFile::m_is_dir = isDir;
}

QString RcloneFile::getSizeString() const
{
    if (m_size == 0)
        return "--";
    return QString::fromStdString(Iridium::Utility::sizeToString(m_size));
}

QString RcloneFile::getPathString() const
{
    QString tmpPath = m_path;
    if (m_is_dir and !m_remote_info->isLocal() and m_path.contains(":"))
    {
        tmpPath = m_path.split(":")[1];
    }
    return QDir(tmpPath).dirName();
}

QString RcloneFile::getModTimeString() const
{
    return QLocale().toString(m_mod_time, tr("dd MMM yyyy - hh:mm:ss"));
}

uint32_t RcloneFile::getObjs() const
{
    return m_objs;
}

void RcloneFile::setObjs(uint32_t objs)
{
    RcloneFile::m_objs = objs;
}

std::shared_ptr<RemoteInfo> RcloneFile::getRemoteInfo() const
{
    return m_remote_info;
}

QString RcloneFile::getFileType() const
{
    if (m_is_dir)
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
    if (not m_remote_info->isLocal() and not m_path.contains("/"))
    {
        m_path = m_path.split(":")[0] + ":" + newName;
        return;
    }
    if (m_is_dir and m_path.endsWith("/"))
        m_path.remove(m_path.size() - 1, 1);
    if (m_path.contains("/"))
        m_path = m_path.remove(m_path.lastIndexOf("/") + 1, m_path.split("/").last().size());
    else if (not m_remote_info->isLocal())
        m_path = m_path.remove(m_path.lastIndexOf(":") + 1, m_path.split(":").last().size());
    m_path.append(newName);
    if (m_is_dir)
        m_path.append("/");
}

QIcon RcloneFile::getIcon()
{
    QIcon ico;
    if (m_is_dir)
    {
        if (QFileInfo(getName()).suffix() == "app")
            ico = QIcon::fromTheme(getName().toLower().remove(".app").replace(" ", "-"),
                                   QIcon::fromTheme("application-default-icon"));
        else ico = Settings::dirIcon();
        m_size = 0;
    } else
    {
        if (QFileInfo(getName()).suffix() == "exe")
            ico = QIcon::fromTheme(getName().toLower().remove(".exe").replace(" ", "-"));
        if (ico.isNull())
            ico = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(getPath()).iconName(),
                                   QIcon::fromTheme(
                                           QMimeDatabase().mimeTypeForFile(getPath()).genericIconName()));
        if (ico.isNull())
            ico = QIcon::fromTheme("unknown");
    }
    return ico;
}

RcloneFile RcloneFile::getParentDir() const
{
    auto rcloneFile = *this;
    auto lst = rcloneFile.getPath().split("/");
    if(lst.size() == 1)
        return rcloneFile;
    if(lst.last().isEmpty())
        lst.removeLast();
    lst.removeLast();
    rcloneFile.m_path = lst.join("/");
    rcloneFile.m_path +="/";
    rcloneFile.setIsDir(true);
    return rcloneFile;
}

