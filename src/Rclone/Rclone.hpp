//
// Created by sr-71 on 09/01/2023.
//


#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include "RcloneFile.hpp"
#include "../Remote/Remote.h"

class Rclone : public QProcess
{
Q_OBJECT
private:
    QString pathRclone{};
public:

    explicit Rclone(QString path);

    explicit Rclone();

    Rclone(const Rclone &rclone);

public:

    [[nodiscard]] const QString &getPathRclone() const;

    void setPathRclone(const QString &pathRclone);

    void config(RemoteType type, const QStringList &params);

    void lsJson(const QString &path);

    void upload(const RcloneFile &src, const RcloneFile &dest);

    void download(const RcloneFile &src, const RcloneFile &dest);

    void deleteRemote(const QString &remote);

    void listRemotes();

    Rclone &operator=(Rclone &&rclone) noexcept;

signals:

    void lsJsonFinished(QJsonDocument doc);

    void uploadData(double value);

    void downloadData(double value);

    void exitCode(int exit);

    void listRemotesFinished(QMap<QString, QString> map);

};


#endif //IRIDIUM_RCLONE_HPP
