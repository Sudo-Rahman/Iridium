//
// Created by sr-71 on 22/06/2024.
//

#include "CryptRemoteConfigParamsFrame.hpp"

#include <Global.hpp>
#include <ranges>

CryptRemoteConfigParamsFrame::CryptRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	_remote_combo = new QComboBox(this);
	_password_1 = new RoundedLineEdit(this);
	_password_1->setEchoMode(QLineEdit::Password);
	_password_1->setClearButtonEnabled(true);
	_password_2 = new RoundedLineEdit(this);
	_password_2->setEchoMode(QLineEdit::Password);
	_password_2->setClearButtonEnabled(true);
	_path = new RoundedLineEdit(this);
	CryptRemoteConfigParamsFrame::createUi();
}

void CryptRemoteConfigParamsFrame::addRemote()
{
	RemoteConfigParamsFrame::addRemote();
	if (not checkFields())
		return;
	auto config = _process->config_create().name(_remote_name->text().toStdString())
			.type(ire::remote::remote_type_to_string(ire::remote::crypt)).add_param(
				"remote=" + _remote_combo->currentData().toString().toStdString() + _path->text().toStdString())
			.add_param("password=" + _password_1->text().toStdString());
	if (_password_2->text().size() > 0)
		config.add_param("password2=" + _password_2->text().toStdString());
	config.execute();
}

void CryptRemoteConfigParamsFrame::createUi()
{
	RemoteConfigParamsFrame::createUi();
	_password_2->setAccessibleName(noCheck);
	_path->setAccessibleName(noCheck);
	_login->setText(tr("Ajouter"));
	_form_layout->addRow(tr("Remote"), _remote_combo);
	_form_layout->addRow(tr("Path"), _path);
	_form_layout->addRow(tr("Mot de passe 1"), _password_1);
	_form_layout->addRow(tr("Mot de passe 2"), _password_2);
	for (const auto &remote: Iridium::Global::remotes | std::views::filter([](const auto &remote)
	{
		return remote->type() != ire::remote::none and remote->type() !=
		       ire::remote::other and remote->type() != ire::remote::alias
		       and remote->type() != ire::remote::crypt;
	}))
		_remote_combo->addItem(remote->getIcon(), remote->name().c_str(), remote->root_path().c_str());
}

bool CryptRemoteConfigParamsFrame::checkFields()
{
	bool ok = RemoteConfigParamsFrame::checkFields();
	ok &= _remote_combo->currentIndex() != -1;
	ok &= _password_1->text().size() > 0;
	return ok;
}

void CryptRemoteConfigParamsFrame::reset()
{
	RemoteConfigParamsFrame::reset();
	_remote_combo->setCurrentIndex(0);
}
