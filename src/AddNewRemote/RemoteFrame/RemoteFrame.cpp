//
// Created by sr-71 on 11/01/2023.
//

#include "RemoteFrame.hpp"

#include <QLayout>
#include <QLabel>
#include <QStyle>
#include <QPainter>
#include <QGraphicsDropShadowEffect>


RemoteFrame::RemoteFrame( const QString & path, RemoteFrame::TypeRemote type, QWidget * parent ) :
        QFrame( parent ), image( { path } ), type( type )
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );


    auto * layout = new QVBoxLayout( this );

    auto * labelIcon = new QLabel;
    labelIcon->setAlignment( Qt::AlignCenter );
    labelIcon->setPixmap( image.scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ));
    layout->addWidget( labelIcon );

    auto * labelRemoteName = new QLabel;
    labelRemoteName->setAlignment( Qt::AlignCenter );
    layout->addWidget( labelRemoteName );
    switch ( RemoteFrame::type )
    {
        case Drive:
            labelRemoteName->setText( "Google Drive" );
            break;

    }
    update();
}

RemoteFrame::TypeRemote RemoteFrame::getType() const
{
    return type;
}

void RemoteFrame::setType( RemoteFrame::TypeRemote type )
{
    RemoteFrame::type = type;
}

const QPixmap & RemoteFrame::getImage() const
{
    return image;
}

void RemoteFrame::setImage( const QPixmap & image )
{
    RemoteFrame::image = image;
}

void RemoteFrame::mousePressEvent( QMouseEvent * event )
{
    QFrame::mousePressEvent( event );
    clickRelease = ! clickRelease;
    update();
}

void RemoteFrame::mouseReleaseEvent( QMouseEvent * event )
{
    QFrame::mouseReleaseEvent( event );
    clickRelease = ! clickRelease;
    update();
    emit clicked();
}

void RemoteFrame::paintEvent( QPaintEvent * event )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    if ( clickRelease )
        painter.setBrush( QColor( 113, 121, 126 ));
    else
        painter.setBrush( QBrush());
    painter.setPen( Qt::gray );

    // Change border radius
    QRect rect = this->rect().marginsRemoved( QMargins( 5, 5, 5, 5 ));
    rect.setWidth( rect.width() - 1 );
    rect.setHeight( rect.height() - 1 );
    painter.drawRoundedRect( rect, 15, 15 );

    QFrame::paintEvent( event );
}
