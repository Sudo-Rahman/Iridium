//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <QCoreApplication>

Rclone::Rclone( QString path ) : pathRclone( std::move( path )) { }

Rclone::Rclone() : pathRclone( QCoreApplication::applicationDirPath().append( "/rclone" )) { }

Rclone::Rclone(const Rclone &rclone)
{
    Rclone::pathRclone = rclone.getPathRclone();

}


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
    QStringList arguments( { "lsjson", path } );
    connect(this, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
        {
            auto doc = QJsonDocument::fromJson( readAllStandardOutput());
            emit lsJsonFinished( doc );
        }
        emit exitCode( exit );
    } );
    start( pathRclone, arguments );
}

/**
 * @brief upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::upload( const RcloneFile & src, const RcloneFile & dest )
{
    QStringList arguments( { "copyto", src.getPath(), dest.getPath() + src.getName(), "-P" } );
    connect( this, & QProcess::readyReadStandardOutput, this, [ &, this ]()
    {
        auto data = QString( readAll().data()).split( "\n" );
        if ( not data.isEmpty())
        {
            auto l1 = data[0].split( QRegularExpression( " |," ));
            erase_if( l1, []( const auto & item ) { return item == "" or item == "\t" or item == ","; } );
            if ( l1.size() > 6 )
                    emit downloadData( l1[6].remove( "%" ).toDouble());
        }
    } );
    connect(this, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
                emit uploadData( 100.0 );
        emit exitCode( exit );
    } );
    start( pathRclone, arguments );

}

/**
 * @brief telecharge le fichier distant src vers la destination local dest.
 * @param src
 * @param dest
 */
void Rclone::download( const RcloneFile & src, const RcloneFile & dest )
{
    QStringList arguments( { "copyto", src.getPath(), dest.getPath() + src.getName(), "-P" } );
    connect(this, & QProcess::readyReadStandardOutput, this, [ &, this ]()
    {
        auto data = QString( readAll().data()).split( "\n" );
        if ( not data.isEmpty())
        {
            auto l1 = data[0].split( QRegularExpression( " |," ));
            erase_if( l1, []( const auto & item ) { return item == "" or item == "\t" or item == ","; } );
            if ( l1.size() > 6 )
                    emit downloadData( l1[6].remove( "%" ).toDouble());
        }
    } );
    connect(this, & QProcess::finished, this, [ =, this ]( int exit )
    {
        if ( exit == 0 )
                emit downloadData( 100.0 );
        emit exitCode( exit );
    } );
    start( pathRclone, arguments );
}

void Rclone::config( RemoteType type, const QStringList & params )
{
    qDebug() << this;

    int * compteur = new int();
    * compteur = 0;

    connect( this, & QProcess::readyReadStandardOutput, this, [ = ]()
    {
        qDebug() << readAllStandardOutput() << * compteur;
        switch ( * compteur )
        {
            case 0:
                write( "n\n" );
                break;
            case 1:
                write( params[0].toLatin1() + "\n" );
                break;
            case 2:
                write( "drive\n" );
            default:
                write( "\n" );
        }
        * compteur += 1;
        if ( * compteur >= 100 )
            exit( - 1 );
    } );
    switch ( type )
    {

        case Drive:
        {
            if ( not params.isEmpty())
            {
                start( pathRclone, { "config","create",params[0], "drive" } );
            }

        }
            break;
    }
}

void Rclone::listRemotes()
{
    connect(this, & QProcess::finished, this, [ = ]( int exit )
    {
        if ( exit == 0 )
        {
            auto data = QString( readAll()).split( "\n" );
            erase_if( data, []( auto & str ) { return str == ""; } );
            QMap < QString, QString > map;
            for ( auto & str: data )
            {
                str.remove( " " );
                map.insert( str.split( ":" )[0], str.split( ":" )[1] );
            }
            emit listRemotesFinished( map );
        }
    } );
    start( pathRclone, { "listremotes", "--long" } );

}

void Rclone::deleteRemote( const QString & remote )
{
    connect(this, & QProcess::finished, this, [ = ]( int exit )
    {
        emit exitCode( exit );
    } );
   start( pathRclone, { "config", "delete", remote } );
}

Rclone &Rclone::operator=(Rclone &&rclone) noexcept
{
    qDebug() << this << &rclone;
    if (this == &rclone)
        return *this;
    qDebug() << this << &rclone;

    this->pathRclone = rclone.pathRclone;
    return *this;
}
