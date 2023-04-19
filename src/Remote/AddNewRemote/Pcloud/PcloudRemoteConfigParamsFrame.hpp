//
// Created by sr-71 on 19/04/2023.
//

#ifndef IRIDIUM_PCLOUDREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_PCLOUDREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class PcloudRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

public:
	explicit PcloudRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void addRemote() override;
};


#endif //IRIDIUM_PCLOUDREMOTECONFIGPARAMSFRAME_HPP
