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

/**
 * @brief Liste sous forme de json les fichiers dans le dossier path
 * @param path
 */
void Rclone::lsJson( const QString & path )
{
    auto * process = new QProcess;
    QStringList arguments( { "lsjson", storageName + path } );
    connect( process, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
        {
            auto doc = QJsonDocument::fromJson( process->readAllStandardOutput());
            emit lsJsonFinished( doc );
        }
        emit exitCode( exit );
        delete process;
    } );
    process->start( pathRclone, arguments );
}

/**
 * @brief upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::upload( const RcloneFile & src, const RcloneFile & dest )
{
    auto * process = new QProcess;
    QStringList arguments( { "copyto", src.getPath(), storageName + dest.getPath() + src.getName(), "-P" } );
    connect( process, & QProcess::readyReadStandardOutput, this, [ &, process, this ]()
    {
        auto data = QString( process->readAll().data()).split( "\n" );
        if ( not data.isEmpty())
        {
            auto l1 = data[0].split( QRegularExpression( " |," ));
            erase_if( l1, []( const auto & item ) { return item == "" or item == "\t" or item == ","; } );
            if ( l1.size() > 6 )
                    emit downloadData( l1[6].remove( "%" ).toDouble());
        }
    } );
    connect( process, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
                emit uploadData( 100.0 );
        emit exitCode( exit );
        delete process;
    } );
    process->start( pathRclone, arguments );

}

/**
 * @brief telecharge le fichier distant src vers la destination local dest.
 * @param src
 * @param dest
 */
void Rclone::download( const RcloneFile & src, const RcloneFile & dest )
{

    auto * process = new QProcess;
    QStringList arguments( { "copyto", storageName + src.getPath(), dest.getPath() + src.getName(), "-P" } );
    connect( process, & QProcess::readyReadStandardOutput, this, [ &, process, this ]()
    {
        auto data = QString( process->readAll().data()).split( "\n" );
        if ( not data.isEmpty())
        {
            auto l1 = data[0].split( QRegularExpression( " |," ));
            erase_if( l1, []( const auto & item ) { return item == "" or item == "\t" or item == ","; } );
            if ( l1.size() > 6 )
                    emit downloadData( l1[6].remove( "%" ).toDouble());
        }
    } );
    connect( process, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
                emit downloadData( 100.0 );
        emit exitCode( exit );
        delete process;
    } );
    process->start( pathRclone, arguments );
}
