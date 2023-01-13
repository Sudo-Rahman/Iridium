//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_GOOGLEDRIVEREMOTEFRAME_HPP
#define IRIDIUM_GOOGLEDRIVEREMOTEFRAME_HPP

#include "RemoteFrame.hpp"

class GoogleDriveRemoteFrame : public RemoteFrame
{
Q_OBJECT
protected:
    void mouseReleaseEvent( QMouseEvent * event ) override;
};


#endif //IRIDIUM_GOOGLEDRIVEREMOTEFRAME_HPP
