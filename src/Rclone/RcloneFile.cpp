//
// Created by Rahman on 10/01/2023.
//

#include "RcloneFile.hpp"

#include <QMimeDatabase>
#include <Settings.hpp>
#include <Utility/Utility.hpp>

void RcloneFile::init()
{
    QFileInfo info(_path);
    if (_remote_info->isLocal())
    {
        setSize(info.size());
        setModTime(info.lastModified());
        setPath(info.absoluteFilePath());

    }
    setIsDir(info.isDir());
    if (_remote_info->isLocal() and _is_dir)
        setPath(_path);
}


RcloneFile::RcloneFile(const QString &pathFile, const RemoteInfoPtr &remoteInfo)
{
    _path = pathFile;
    _remote_info = remoteInfo;
    init();
}

RcloneFile::RcloneFile(const QString &pathFile, uint64_t size, bool isDir, QDateTime modTime,
                       const RemoteInfoPtr &remoteInfo)
        : _size(size), _is_dir(isDir), _mod_time(std::move(modTime))
{
    _remote_info = remoteInfo;
    setPath(pathFile);
}


const QString &RcloneFile::getPath() const
{
    return _path;
}

void RcloneFile::setPath(const QString &path)
{
    RcloneFile::_path = path;
    if (not path.endsWith("/") and not path.isEmpty() and _is_dir)
        RcloneFile::_path += "/";
}

uint64_t RcloneFile::getSize() const
{
    return _size;
}

void RcloneFile::setSize(uint64_t size)
{
    RcloneFile::_size = size;
}

const QDateTime &RcloneFile::getModTime() const
{
    return _mod_time;
}

void RcloneFile::setModTime(const QDateTime &modTime)
{
    RcloneFile::_mod_time = modTime;
}

QString RcloneFile::getName() const
{
    QString name;
    if (_is_dir)
        return getPathString();
    _path.contains(":") ? name = QFileInfo(_path.split(":")[1]).fileName() : name = QFileInfo(_path).fileName();
    return name;
}

bool RcloneFile::isDir() const
{
    return _is_dir;
}

void RcloneFile::setIsDir(bool isDir)
{
    if (_path.endsWith(":"))
        RcloneFile::_is_dir = true;
    else
        RcloneFile::_is_dir = isDir;
}

QString RcloneFile::getSizeString() const
{
    if (_size == 0)
        return "--";
    return QString::fromStdString(Iridium::Utility::sizeToString(_size));
}

QString RcloneFile::getPathString() const
{
    QString tmpPath = _path;
    if (_is_dir and !_remote_info->isLocal() and _path.contains(":"))
    {
        tmpPath = _path.split(":")[1];
    }
    return QDir(tmpPath).dirName();
}

QString RcloneFile::getModTimeString() const
{
    return QLocale().toString(_mod_time, tr("dd MMM yyyy - hh:mm:ss"));
}

uint32_t RcloneFile::getObjs() const
{
    return _objs;
}

void RcloneFile::setObjs(uint32_t objs)
{
    RcloneFile::_objs = objs;
}

std::shared_ptr<RemoteInfo> RcloneFile::getRemoteInfo() const
{
    return _remote_info;
}

QString RcloneFile::getFileType() const
{
    if (_is_dir)
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
    if (not _remote_info->isLocal() and not _path.contains("/"))
    {
        _path = _path.split(":")[0] + ":" + newName;
        return;
    }
    if (_is_dir and _path.endsWith("/"))
        _path.remove(_path.size() - 1, 1);
    if (_path.contains("/"))
        _path = _path.remove(_path.lastIndexOf("/") + 1, _path.split("/").last().size());
    else if (not _remote_info->isLocal())
        _path = _path.remove(_path.lastIndexOf(":") + 1, _path.split(":").last().size());
    _path.append(newName);
    if (_is_dir)
        _path.append("/");
}

QIcon RcloneFile::getIcon()
{
    QIcon ico;
    if (_is_dir)
    {
        if (QFileInfo(getName()).suffix() == "app")
            ico = QIcon::fromTheme(getName().toLower().remove(".app").replace(" ", "-"),
                                   QIcon::fromTheme("application-default-icon"));
        else ico = Settings::dirIcon();
        _size = 0;
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
    if (lst.size() == 1)
        return rcloneFile;
    if (lst.last().isEmpty())
        lst.removeLast();
    lst.removeLast();
    rcloneFile._path = lst.join("/");
    rcloneFile._path += "/";
    rcloneFile.setIsDir(true);
    return rcloneFile;
}

RcloneFile RcloneFile::fromJson(const boost::json::object &json, const RemoteInfoPtr &remoteInfo)
{
    try
    {
        auto path = remoteInfo->path + json.at("Path").as_string().c_str();
        auto size = json.at("Size").as_int64();
        auto dir = json.at("IsDir").as_bool();
        auto modTime = QDateTime::fromString(json.at("ModTime").as_string().c_str(), Qt::ISODateWithMs);
        return {path.c_str(), (uint64_t)size, dir, modTime, remoteInfo};
    } catch (boost::exception &e)
    {
        std::cout << "Error while parsing json: " << diagnostic_information_what(e, true) << std::endl;
    }
    return {"null",remoteInfo};
}

