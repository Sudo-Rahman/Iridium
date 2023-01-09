//
// Created by sr-71 on 09/01/2023.
//

#include <QString>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QThread>

#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP


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

    void upload( const QString & src, const QString & dest );

    void download( const QString & src, const QString & dest );

signals:

    void lsJsonFinished( QJsonDocument doc );

    void uploadData( double value );


};


#endif //IRIDIUM_RCLONE_HPP
