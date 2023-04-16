//
// Created by sr-71 on 16/04/2023.
//

#ifndef IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class OpenDriveRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

	QLineEdit *m_userLineEdit{}, *m_passwordLineEdit{};

public:
	explicit OpenDriveRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void createUi() override;

	void addRemote() override;

};


#endif //IRIDIUM_OPENDRIVEREMOTECONFIGPARAMSFRAME_HPP