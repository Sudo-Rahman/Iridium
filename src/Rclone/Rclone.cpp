//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <QFile>

Rclone::Rclone( QString path ) : pathRclone( std::move( path )) { loadListRemotes(); }

const QString & Rclone::getPathRclone() const
{
    return pathRclone;
}

void Rclone::setPathRclone( const QString & pathRclone )
{
    Rclone::pathRclone = pathRclone;
}


/**
 * @brief Liste sous forme de json les fichiers dans le dossier path
 * @param path
 */
void Rclone::lsJson( const QString & path )
{
    auto * process = new QProcess;
    QStringList arguments( { "lsjson", path } );
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
    QStringList arguments( { "copyto", src.getPath(), dest.getPath() + src.getName(), "-P" } );
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
    QStringList arguments( { "copyto", src.getPath(), dest.getPath() + src.getName(), "-P" } );
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

void Rclone::config( Rclone::Config config, const QStringList & params )
{
    int * compteur = new int();
    * compteur = 0;
    auto * process = new QProcess;
    connect( process, & QProcess::readyReadStandardOutput, this, [ = ]()
    {
        qDebug() << process->readAllStandardOutput() << * compteur;
        switch ( * compteur )
        {
            case 0:
                process->write( "n\n" );
                break;
            case 1:
                process->write( params[0].toLatin1() + "\n" );
                break;
            case 2:
                process->write( "drive\n" );
            default:
                process->write( "\n" );
        }
        * compteur += 1;
        if ( * compteur >= 100 )
            exit( - 1 );
    } );
    switch ( config )
    {

        case Drive:
        {
            if ( not params.isEmpty())
            {
//                process->start( pathRclone, { "config","create",params[0], "drive" } );
                process->start( pathRclone, { "config" } );
                process->waitForStarted();
            }

        }
            break;
    }
    loadListRemotes();
}

void Rclone::loadListRemotes()
{
    auto * process = new QProcess;
    connect( process, & QProcess::finished, this, [ = ]( int exit )
    {
        if ( exit == 0 )
        {
            auto data = QString( process->readAll()).split( "\n" );
            erase_if( data, []( auto & str ) { return str == ""; } );
            QMap < QString, QString > map;
            for ( auto & str: data )
            {
                str.remove( " " );
                map.insert( str.split( ":" )[0], str.split( ":" )[1] );
            }
            listRemotes = map;
            emit listRemotesFinished( map );
        }
    } );
    process->start( pathRclone, { "listremotes", "--long" } );

}

const QMap < QString, QString > & Rclone::getListRemotes()
{
    return listRemotes;
}

void Rclone::deleteRemote( const QString & remote )
{
    auto * process = new QProcess;
    connect( process, & QProcess::finished, this, [ = ]( int exit )
    {
        emit exitCode( exit );
    } );
    process->start( pathRclone, { "config", "delete", remote } );
}
