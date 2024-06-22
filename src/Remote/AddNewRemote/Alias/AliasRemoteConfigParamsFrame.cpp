//
// Created by sr-71 on 22/06/2024.
//

#include "AliasRemoteConfigParamsFrame.hpp"
#include <ranges>

AliasRemoteConfigParamsFrame::AliasRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	_remote_combo = new QComboBox(this);
	_alias_name = new RoundedLineEdit(this);
	AliasRemoteConfigParamsFrame::createUi();
}

void AliasRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();
	_login->setText(tr("Ajouter"));
	_form_layout->addRow(tr("Remote"), _remote_combo);
	_form_layout->addRow(tr("Alias"), _alias_name);
	for (const auto &remote: Iridium::Global::remotes | std::views::filter([](const auto &remote)
	{
		return remote->type() != ire::remote::none and remote->type() !=
		       ire::remote::other and remote->type() != ire::remote::alias;
	}))
		_remote_combo->addItem(remote->getIcon(), remote->name().c_str(), remote->root_path().c_str());
}

void AliasRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	_process->config_create().name(_remote_name->text().toStdString())
			.type(ire::remote::remote_type_to_string(ire::remote::alias)).add_param(
				"remote=" + _remote_combo->currentData().toString().toStdString()  + _alias_name->text().toStdString())
			.execute();
}

bool AliasRemoteConfigParamsFrame::checkFields()
{
	bool ok = RemoteConfigParamsFrame::checkFields();
	ok &= _remote_combo->currentIndex() != -1;
	ok &= _alias_name->text().size() > 0;
	return ok;
}

void AliasRemoteConfigParamsFrame::reset()
{
	RemoteConfigParamsFrame::reset();
	_remote_combo->setCurrentIndex(0);
}
