//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_ONEDRIVEREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_ONEDRIVEREMOTECONFIGPARAMSFRAME_HPP

#include "../RemoteConfigParamsFrame.hpp"

class OneDriveRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

public:
    explicit OneDriveRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
    void addRemote() override;

    bool checkFields() override;

};


#endif //IRIDIUM_ONEDRIVEREMOTECONFIGPARAMSFRAME_HPP
