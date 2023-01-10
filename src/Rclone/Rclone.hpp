//
// Created by sr-71 on 09/01/2023.
//


#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP

#include <QString>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QThread>
#include "RcloneFile.hpp"

class Rclone : public QObject
{
Q_OBJECT
private:
    QString pathRclone {};
    QString storageName {};
public:

    explicit Rclone( QString path, QString name );

public:
    const QString & getStorageName() const;

    void setStorageName( const QString & storageName );

    [[nodiscard]] const QString & getPathRclone() const;

    void setPathRclone( const QString & pathRclone );

    void lsJson( const QString & path );

    void upload( const RcloneFile & src, const RcloneFile & dest );

    void download( const RcloneFile & src, const RcloneFile & dest );

signals:

    void lsJsonFinished( QJsonDocument doc );

    void uploadData( double value );

    void downloadData( double value );

    void exitCode(int exit);


};


#endif //IRIDIUM_RCLONE_HPP
