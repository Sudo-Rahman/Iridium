//
// Created by Rahman on 08/04/2023.
//

#ifndef IRIDIUM_SFTPREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_SFTPREMOTECONFIGPARAMSFRAME_HPP

#include "../RemoteConfigParamsFrame.hpp"


class SftpRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    QLineEdit *m_host{}, *m_user{}, *m_password{};
    QSpinBox *m_port{};

public:
    explicit SftpRemoteConfigParamsFrame(QWidget *parent = nullptr);

    void reset() override;

protected:
    void addRemote() override;

    void createUi() override;

};


#endif //IRIDIUM_SFTPREMOTECONFIGPARAMSFRAME_HPP
