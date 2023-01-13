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
    QMap < QString, QString > listRemotes;
public:

    explicit Rclone( QString path );

    enum Config
    {
        Drive, Sftp
    };

public:

    [[nodiscard]] const QString & getPathRclone() const;

    void setPathRclone( const QString & pathRclone );

    void config( Config config, const QStringList & params );

    void lsJson( const QString & path );

    void upload( const RcloneFile & src, const RcloneFile & dest );

    void download( const RcloneFile & src, const RcloneFile & dest );

    void deleteRemote( const QString & remote );

    [[nodiscard]] const QMap < QString, QString > & getListRemotes();

private:
    void loadListRemotes();

signals:

    void lsJsonFinished( QJsonDocument doc );

    void uploadData( double value );

    void downloadData( double value );

    void exitCode( int exit );

    void listRemotesFinished( QMap < QString, QString > map );


};


#endif //IRIDIUM_RCLONE_HPP
