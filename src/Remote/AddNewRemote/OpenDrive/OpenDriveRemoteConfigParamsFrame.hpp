//
// Created by Rahman on 16/04/2023.
//

#ifndef IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class OpenDriveRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    RoundedLineEdit *_user{}, *_password{};

public:
    explicit OpenDriveRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
    void createUi() override;

    void addRemote() override;

};


#endif //IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP
