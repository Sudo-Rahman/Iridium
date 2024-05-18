//
// Created by sr-71 on 06/06/2023.
//

#include "CmdRemoteConfigParamsFrame.hpp"

CmdRemoteConfigParamsFrame::CmdRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	CmdRemoteConfigParamsFrame::createUi();
}

void CmdRemoteConfigParamsFrame::createUi()
{
	_layout->setContentsMargins(20, 20, 20, 20);
	_cmd_textEdit = new CmdTextEdit(this);
	_layout->addWidget(_cmd_textEdit);
}
