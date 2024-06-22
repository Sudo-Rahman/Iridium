//
// Created by sr-71 on 22/06/2024.
//
#pragma once

#include <RemoteConfigParamsFrame.hpp>

class CryptRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
	Q_OBJECT

	QComboBox *_remote_combo{};
	RoundedLineEdit *_path{};
	RoundedLineEdit *_password_1{};
	RoundedLineEdit *_password_2{};

public:
	CryptRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	void addRemote() override;

	void createUi() override;

	bool checkFields() override;

	void reset() override;
};
