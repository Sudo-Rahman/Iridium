//
// Created by sr-71 on 06/06/2023.
//

#ifndef IRIDIUM_CMDREMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_CMDREMOTECONFIGPARAMSFRAME_HPP

#include "RemoteConfigParamsFrame.hpp"
#include "CmdTextEdit.hpp"

class CmdRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
Q_OBJECT

    CmdTextEdit *_cmd_textEdit{};

public:
    explicit CmdRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:

    void createUi() override;

};


#endif //IRIDIUM_CMDREMOTECONFIGPARAMSFRAME_HPP
