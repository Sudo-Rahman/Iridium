//
// Created by sr-71 on 05/04/2023.
//

#ifndef IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP

#include "../RemoteConfigParamsFrame.hpp"

class LocalRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

private:
	QString m_path{};

public:
	explicit LocalRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void addRemote() override;

	void createUi() override;

};


#endif //IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP
