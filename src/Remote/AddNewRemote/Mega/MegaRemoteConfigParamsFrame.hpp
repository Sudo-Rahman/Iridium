//
// Created by Rahman on 15/04/2023.
//

#ifndef IRIDIUM_MEGAREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_MEGAREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"

class MegaRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    QLineEdit *_user{}, *_password{};

public:
    explicit MegaRemoteConfigParamsFrame(QWidget *parent = nullptr);


protected:
    void createUi() override;

    void addRemote() override;
};


#endif //IRIDIUM_MEGAREMOTECONFIGPARAMSFRAME_HPP
