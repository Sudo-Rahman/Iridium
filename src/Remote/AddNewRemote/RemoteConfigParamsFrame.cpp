//
// Created by rahman on 13/01/23.
//

#include <QPainter>
#include <QMessageBox>
#include <QIterator>
#include <ranges>
#include <IridiumApp.hpp>

#include "RemoteConfigParamsFrame.hpp"

using namespace std;

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{
	_layout = new QVBoxLayout(this);
	_layout->setSpacing(10);
}

/**
 * @brief creation of the ui
 */
void RemoteConfigParamsFrame::createUi()
{
	_form_layout = new QFormLayout;
	_form_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
	_form_layout->setFormAlignment(Qt::AlignTop);
	_form_layout->setContentsMargins(0, 0, 0, 0);
	_layout->addLayout(_form_layout);

	_remote_name = new RoundedLineEdit(this);
	_form_layout->addRow(tr("Nom : "), _remote_name);

	auto *vertlay = new QVBoxLayout;
	vertlay->setAlignment(Qt::AlignTop | Qt::AlignCenter);
	vertlay->setContentsMargins(0, 0, 0, 0);
	vertlay->setSpacing(0);
	auto *horlayout = new QHBoxLayout;
	vertlay->addLayout(horlayout);

	_login = new QPushButton(tr("Se connecter"), this);
	_login->setDefault(true);
	_login->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	horlayout->addWidget(_login, 0, Qt::AlignTop);

	_cancel = new QPushButton(tr("Annuler"), this);
	_cancel->hide();
	_cancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	horlayout->addWidget(_cancel, 0, Qt::AlignTop);

	_layout->addLayout(vertlay);

	connect(_login, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);

	connect(_cancel, &QPushButton::clicked, this, [this]()
	{
		_process->stop();
		_cancel->hide();
		_login->show();
	});

	_mess_label = new QLabel(this);
	_mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
	_mess_label->hide();
	_mess_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_mess_label->setAutoFillBackground(true);
	QPalette p;
	p.setColor(QPalette::WindowText, Qt::red);
	_mess_label->setPalette(p);
	vertlay->addWidget(_mess_label, 0, Qt::AlignTop);

	for (auto &field: findChildren<RoundedLineEdit *>())
	{
		connect(field, &QLineEdit::textChanged, this, [this, field]()
		{
			_mess_label->hide();
			field->normalBorder();
		});
	}
}

/**
 * @brief add a new remote  function called when the user click on the login button
 */
void RemoteConfigParamsFrame::addRemote()
{
	_process = std::make_unique<ir::process>();

	if (_remote_name->text().isEmpty())
	{
		_remote_name->redBorder();
		_mess_label->show();
		_mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
		return;
	}

	_process
			->on_start([this]
			{
				_login->hide();
				_cancel->show();
			})
			.on_finish([this](int exit)
			{
				IridiumApp::runOnMainThread([exit,this]
				{
					if (exit == 0)
					{
						emit remoteAdded();
						QMessageBox::information(this, tr("Succès"),
						                         tr("Le disque %1 a été ajouté avec succès").arg(_remote_name->text()));
						clearAllFields();
					}
					else
					{
						auto msgBox = QMessageBox(this);
						msgBox.setWindowTitle(tr("Erreur"));
						msgBox.setText(tr("Une erreur est survenue lors de la configuration du serveur distant !"));
						msgBox.setStandardButtons(QMessageBox::Ok);
						msgBox.exec();
					}
					_cancel->hide();
					_login->show();
				});
			});

	std::vector<RemoteInfoPtr> rclone_liste_remote;
	iridium::rclone::process().list_remotes([this](const std::vector<ire::remote_ptr> &remotes)
	{
		_remotes.clear();
		for (const auto &remote: remotes)
		{
			_remotes.push_back(std::make_shared<RemoteInfo>(remote->name(), remote->type(), remote->path()));
		}
	});
}

/**
 * @brief check if all fields are filled
 * @return true if all fields are filled
 */
bool RemoteConfigParamsFrame::checkFields()
{
	bool ok = true;
	for (auto &remote: _remotes)
	{
		if (_remote_name->text().toStdString() == remote->name())
		{
			QMessageBox::critical(this, tr("Erreur"), tr("Le nom du disque est déjà utilisé !"));
			return false;
		}
	}
	for (auto &field: findChildren<RoundedLineEdit *>()
	                  | std::views::filter([](auto field) { return field->accessibleName() != "noCheck"; })
		)
	{
		if (field->text().isEmpty())
		{
			field->redBorder();
			_mess_label->show();
			_mess_label->setText(tr("Les champs en rouge sont obligatoires !"));
			ok = false;
		}
	}
	return ok;
}

/**
 * @brief clear all fields
 */
void RemoteConfigParamsFrame::clearAllFields()
{
	for (auto lineEdit: findChildren<RoundedLineEdit *>()) { lineEdit->reset(); }
}

/**
 * @brief reset the frame
 */
void RemoteConfigParamsFrame::reset()
{
	clearAllFields();
	if (_mess_label)
		_mess_label->hide();
}
