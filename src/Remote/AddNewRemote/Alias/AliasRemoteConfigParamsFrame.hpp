#pragma once

#include <RemoteConfigParamsFrame.hpp>
#include <QComboBox>
#include <Global.hpp>

class AliasRemoteConfigParamsFrame : public RemoteConfigParamsFrame
{
	Q_OBJECT

	QComboBox *_remote_combo{};
	RoundedLineEdit *_alias_name{};

public:
	explicit AliasRemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:

	void createUi() override;

	void addRemote() override;

	bool checkFields() override;

	void reset() override;
};
