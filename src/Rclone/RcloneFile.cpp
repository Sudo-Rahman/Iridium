//
// Created by Rahman on 10/01/2023.
//

#include "RcloneFile.hpp"
#include <QObject>
#include <Settings.hpp>
#include <Utility/Utility.hpp>
#include <QMimeDatabase>

RcloneFile::RcloneFile(file * parent, const QString & file_name, int64_t size, bool is_dir,
    const QDateTime & mod_time, const RemoteInfoPtr & remote)
{
    set_parent(parent);
    set_name(file_name.toStdString());
    set_size(size);
    set_is_dir(is_dir);
    set_mod_time(boost::posix_time::from_time_t(mod_time.toSecsSinceEpoch()));
    set_remote(remote);
}

RcloneFile::RcloneFile(const ire::file & file)
{
    set_parent(file.parent());
    set_name(file.name());
    set_size(file.size());
    set_is_dir(file.is_dir());
    set_mod_time(file.mod_time());
    set_remote(file.remote());
}

const QString RcloneFile::getPath() const
{
    return QString::fromStdString(path());
}

void RcloneFile::setName(const QString &path)
{
    set_name(path.toStdString());
}

uint64_t RcloneFile::getSize() const
{
    return size();
}

void RcloneFile::setSize(uint64_t size)
{
   set_size(std::move(size));
}

const QDateTime RcloneFile::getModTime() const
{
    return QDateTime::fromSecsSinceEpoch(to_time_t(mod_time()));
}

void RcloneFile::setModTime(const QDateTime &modTime)
{
    set_mod_time(boost::posix_time::from_time_t(modTime.toSecsSinceEpoch()));
}

QString RcloneFile::getName() const
{
    return QString::fromStdString(name());
}

bool RcloneFile::isDir() const
{
    return is_dir();
}

void RcloneFile::setIsDir(bool isDir)
{
    set_is_dir(isDir);
}

QString RcloneFile::getSizeString() const
{
    return QString::fromStdString(Iridium::Utility::sizeToString(size()));
}

QString RcloneFile::getPathString() const
{
    return QString::fromStdString(path());
}

QString RcloneFile::getModTimeString() const
{
    return QLocale().toString(getModTime(), QObject::tr("dd MMM yyyy - hh:mm:ss"));
}

uint32_t RcloneFile::getObjs() const
{
    return nb_chilchren();
}

RemoteInfoPtr RcloneFile::getRemoteInfo() const
{
    return dynamic_pointer_cast<RemoteInfo>(remote());
}

QString RcloneFile::getFileType() const
{
    if (isDir())
        return QObject::tr("Dossier");
    auto mime = QMimeDatabase().mimeTypeForFile(getPath());
    if (mime.name() != "application/octet-stream")
    {
        auto type = mime.comment();
        type.front() = type.front().toUpper();
        return type;
    }
    return QObject::tr("Document ") + QFileInfo(getPath()).suffix().toUpper();
}

QIcon RcloneFile::getIcon()const
{
    QIcon ico;
    if (isDir())
    {
        if (QFileInfo(getName()).suffix() == "app")
            ico = QIcon::fromTheme(getName().toLower().remove(".app").replace(" ", "-"),
                                   QIcon::fromTheme("application-default-icon"));
        else ico = Settings::dirIcon();
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

QList<QMimeType> RcloneFile::mimeTypes() const
{
    return QMimeDatabase().mimeTypesForFileName(getPath());
}

