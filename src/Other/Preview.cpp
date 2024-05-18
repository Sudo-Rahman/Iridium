//
// Created by sr-71 on 11/05/2024.
//

#include "Preview.hpp"

#include <QMessageBox>
#include <QProcess>
#include <boost/process.hpp>
#include <Settings.hpp>


#ifdef _WIN32 || _WIN64
#include <boost/process/windows.hpp>
#endif

Preview::Preview(const QString &file, const QByteArray &data) : _data(data), _name(file)
{
    preview();
}

Preview::Preview(unique_file file) : _file(std::move(file))
{
    preview();
}

void Preview::preview()
{
    if (_file == nullptr)
    {
        auto file = new QFile(Settings::saveFileToTemp(_data, _name));
        _file = unique_file(file);
    }
    try
    {
        boost::process::child proc(boost::process::shell,
#ifdef _WIN32 || _WIN64
                                   "powershell /c start",
#elif __APPLE__
                                   "open",
#elif __linux__
            "xdg-open",
#endif
                                   boost::process::args = {
                                       QFileInfo(_file->fileName()).absoluteFilePath().toStdString()
                                   }
#ifdef _WIN32 || _WIN64
                                   , boost::process::windows::hide
#endif

        );
        proc.wait();
        if (proc.exit_code() not_eq 0)
            QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                                  QObject::tr("Impossible d'ouvrir le fichier : ") + _name);
    } catch (std::exception &e) { QMessageBox::critical(nullptr, QObject::tr("Erreur"), e.what()); }
}

bool Preview::isPreviewable(const RcloneFile &file)
{

    if (file.isDir())
        return false;

    if (file.getRemoteInfo()->isLocal())
        return true;

    // max 50 Mo
    if (file.getSize() > 1024 * 1024 * 50 or file.getSize() == 0)
        return false;

    return std::ranges::any_of(file.mimeTypes(), [](const auto &i)
    {
        return i.name().contains("image") || i.name().contains("video") ||
               i.name().contains("audio") || i.name().contains("text") ||
               i.name().contains("pdf");
    });
}

bool Preview::isPreviewable(const QList<RcloneFilePtr> &files)
{
    return std::ranges::all_of(files, [](const auto &i) { return isPreviewable(*i); });
}
