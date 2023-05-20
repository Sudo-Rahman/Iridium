//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class FtpRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    QSpinBox *_port{};
    QLineEdit *_user{}, *_password{}, *_host{};
    QComboBox *_tls{};

public:
    explicit FtpRemoteConfigParamsFrame(QWidget *parent = nullptr);

    void reset() override;

protected:

    void createUi() override;

    void addRemote() override;

};


#endif //IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP
