//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class FtpRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    QSpinBox *m_portSpinBox{};
    QLineEdit *m_userLineEdit{};
    QLineEdit *m_passwordLineEdit{};
    QLineEdit *m_hostLineEdit{};
    QComboBox *m_tls{};

public:
    explicit FtpRemoteConfigParamsFrame(QWidget *parent = nullptr);

    void reset() override;

protected:

    void createUi() override;

    void addRemote() override;

};


#endif //IRIDIUM_FTPREMOTECONFIGPARAMSFRAME_HPP
