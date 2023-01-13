//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"
#include "../Rclone/Rclone.hpp"
#include "../AddNewRemote/RemoteFrame/RemoteFrame.hpp"

#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include <QProgressBar>
#include <QGraphicsBlurEffect>


MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent )
{
    setWindowTitle( "Iridium" );


    auto * rc = new Rclone( rclonePath );
    auto * wid = new QWidget;

    setCentralWidget( wid );

    auto * layout = new QGridLayout( wid );
    auto * btn = new QPushButton( tr( "hello" ));
    layout->addWidget( btn );

    auto * txt = new QTextEdit;
    layout->addWidget( txt );

    auto * bar = new QProgressBar;
    layout->addWidget( bar );

    layout->addWidget( new RemoteFrame( ":/images/google_drive.png", RemoteFrame::Drive ));


    connect( rc, & Rclone::lsJsonFinished, this,
             [ = ]( const QJsonDocument & doc )
             {
//                 txt->setText( doc.toJson( QJsonDocument::Indented ));
             } );
    rc->lsJson( "nas:" );

    connect( btn, & QPushButton::clicked, this, [ btn ]()
    {
        btn->setText( [ btn ]()
                      {
                          QString txt;
                          txt = btn->text() + "o";
                          return txt;
                      }.operator()());
    } );
//    bar->setMaximum( 100 );
//    rc->download( { "nas:firebase_cpp_sdk_10.3.0.zip", RcloneFile::Distant }, { "/Users/sr-71/Downloads/" } );
//    connect( rc, & Rclone::downloadData, this, [ = ]( const double & val ) { bar->setValue(( int ) val ); } );
}
