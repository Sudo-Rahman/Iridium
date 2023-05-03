//
// Created by Rahman on 29/04/2023.
//

#ifndef IRIDIUM_BOXREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_BOXREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class BoxRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

public:
    explicit BoxRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
    void addRemote() override;

};


#endif //IRIDIUM_BOXREMOTECONFIGPARAMSFRAME_HPP
