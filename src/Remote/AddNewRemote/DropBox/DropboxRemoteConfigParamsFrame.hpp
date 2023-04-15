//
// Created by sr-71 on 14/04/2023.
//

#ifndef IRIDIUM_DROPBOXREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_DROPBOXREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class DropboxRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

public:
	explicit DropboxRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void addRemote() override;

};


#endif //IRIDIUM_DROPBOXREMOTECONFIGPARAMSFRAME_HPP
