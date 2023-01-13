//
// Created by sr-71 on 11/01/2023.
//

#include "GoogleDriveRemoteFrame.hpp"

#include <QMouseEvent>

void GoogleDriveRemoteFrame::mouseReleaseEvent( QMouseEvent * event )
{
    QFrame::mouseReleaseEvent( event );
    if ( event->button() == Qt::LeftButton )
    {

    }

}
