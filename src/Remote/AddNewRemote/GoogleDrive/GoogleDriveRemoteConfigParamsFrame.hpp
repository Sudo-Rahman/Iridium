//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_GOOGLEDRIVEREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_GOOGLEDRIVEREMOTECONFIGPARAMSFRAME_HPP

#include "../RemoteConfigParamsFrame.hpp"

class GoogleDriveRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

public:
	explicit GoogleDriveRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void addRemote() override;

	void createUi() override;

	bool checkFields() override;

};


#endif //IRIDIUM_GOOGLEDRIVEREMOTECONFIGPARAMSFRAME_HPP
