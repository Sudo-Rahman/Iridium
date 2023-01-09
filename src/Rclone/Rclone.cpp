//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <QFile>

Rclone::Rclone( QString path, QString name ) : pathRclone( std::move( path ))
{
    setStorageName( name );
}

const QString & Rclone::getPathRclone() const
{
    return pathRclone;
}

void Rclone::setPathRclone( const QString & pathRclone )
{
    Rclone::pathRclone = pathRclone;
}

const QString & Rclone::getStorageName() const
{
    return storageName;
}

void Rclone::setStorageName( const QString & storageName )
{
    Rclone::storageName = storageName;
    if ( ! storageName.endsWith( ':' ))
        Rclone::storageName += ':';
}

void Rclone::lsJson( const QString & path )
{
    auto * process = new QProcess;
    QStringList arguments( { "lsjson", storageName + path } );
    connect( process, & QProcess::finished, this, [ =, this ]()
    {
        auto doc = QJsonDocument::fromJson( process->readAllStandardOutput());
        emit lsJsonFinished( doc );
        delete process;
    } );
    process->start( pathRclone, arguments );
}

void Rclone::upload( const QString & src, const QString & dest )
{
    auto * process = new QProcess;
    QFile file( src );
    QStringList arguments( { "copyto", src, storageName + dest + file.fileName(), "-P" } );
    connect( process, & QProcess::readyReadStandardOutput, this, [ =, this ]()
    {
        QFile file( src );

        QByteArrayList data = process->readAll().split( ' ' );
        int i = 0;
        for ( const auto & str: data )
        {
            bool ok;
            double val = str.toDouble( & ok );
            double progress = 0;
            if ( ok and data[i + 1].contains( "B" ))
            {
                if ( data[i + 1].toLower() == "b" )
                    progress = val / file.size() * 100;
                else
                {
                    if ( data[i + 1].toLower() == "kib" )
                        progress = ( val * 1024 ) / file.size() * 100;
                    else
                    {
                        if ( data[i + 1].toLower() == "mib" )
                            progress = ( val * pow( 1024, 2 )) / file.size() * 100;
                    }
                }
                emit uploadData( progress );
                break;
            }
            i ++;
        }
    } );
    connect( process, & QProcess::finished, this, [ =, this ]()
    {
        emit uploadData( 100.0 );
        delete process;
    } );
    process->start( pathRclone, arguments );

}
