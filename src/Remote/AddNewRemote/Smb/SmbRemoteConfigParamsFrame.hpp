//
// Created by Rahman on 29/04/2023.
//

#ifndef IRIDIUM_SMBREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_SMBREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class SmbRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    QLineEdit *m_host{}, *m_user{}, *m_password{}, *m_domain{};
    QSpinBox *m_port{};

public:
    explicit SmbRemoteConfigParamsFrame(QWidget *parent = nullptr);

    void reset() override;

protected:
    void addRemote() override;

    void createUi() override;

};


#endif //IRIDIUM_SMBREMOTECONFIGPARAMSFRAME_HPP
