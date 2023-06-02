//
// Created by Rahman on 05/04/2023.
//

#ifndef IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP

#include "../RemoteConfigParamsFrame.hpp"

class LocalRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

private:
    QString _path{};
    QPushButton *_path_button{};
    RoundedLineEdit *_path_lineEdit{};


public:
    explicit LocalRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
    void addRemote() override;

    void createUi() override;


};


#endif //IRIDIUM_LOCALREMOTECONFIGPARAMSFRAME_HPP
