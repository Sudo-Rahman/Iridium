//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"
#include "../Rclone/Rclone.hpp"

#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include <QDir>
#include <QProgressBar>

MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent )
{
    auto * rc = new Rclone( QDir::currentPath().append( "/rclone" ), "nas" );
    auto * wid = new QWidget;

    setCentralWidget( wid );

    auto * layout = new QGridLayout( wid );
    auto * btn = new QPushButton( "hello" );
    layout->addWidget( btn );

    auto * txt = new QTextEdit;
    layout->addWidget( txt );

    auto * bar = new QProgressBar;
    layout->addWidget( bar );


    connect( rc, & Rclone::lsJsonFinished, this,
             [ = ]( const QJsonDocument & doc )
             {
                 txt->setText( doc.toJson( QJsonDocument::Indented ));
             } );
    rc->lsJson( "" );

    connect( btn, & QPushButton::clicked, this, [ btn ]()
    {
        btn->setText( [ btn ]()
                      {
                          QString txt;
                          txt = btn->text() + "o";
                          return txt;
                      }.operator()());
    } );
    QFileInfo f( "/Users/sr-71/Downloads/firebase_cpp_sdk_10.3.0.zip" );
    bar->setMaximum( 100 );
    rc->upload( "/Users/sr-71/Downloads/firebase_cpp_sdk_10.3.0.zip", "" );
    connect( rc, & Rclone::uploadData, this, [ = ]( const double & val ) { bar->setValue(( int ) val ); } );
}
