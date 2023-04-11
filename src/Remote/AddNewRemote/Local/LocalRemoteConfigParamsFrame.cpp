//
// Created by sr-71 on 05/04/2023.
//

#include <QFileDialog>
#include <QMessageBox>
#include "LocalRemoteConfigParamsFrame.hpp"
#include <Settings.hpp>

LocalRemoteConfigParamsFrame::LocalRemoteConfigParamsFrame(QWidget *parent) : RemoteConfigParamsFrame(parent)
{
	LocalRemoteConfigParamsFrame::createUi();
}

void LocalRemoteConfigParamsFrame::createUi()
{

	auto *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	m_layout->addLayout(layout);

	auto *btn = new QPushButton(tr("Chemin du dossier"), this);
	layout->addWidget(btn);

	auto *path = new QLineEdit(this);
	path->setPlaceholderText("/user/home");
	connecLineEdit(path);
	layout->addWidget(path);

	connect(btn, &QPushButton::clicked, this, [this, path]()
	{
		auto result = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"),
														QDir::homePath(),
														QFileDialog::ShowDirsOnly |
														QFileDialog::DontResolveSymlinks);
		if (!result.isEmpty())
		{
			m_path = result;
			path->setText(result);
		}
	});

	connect(path, &QLineEdit::textChanged, this, [this, path](const QString &text)
	{
		m_path = text;
		if (QFileInfo::exists(text))
		{
			path->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
			path->setToolTip("");
		} else
		{
			path->setStyleSheet("border: 1px solid red; border-radius: 5px;");
			path->setToolTip(tr("Le chemin n’existe pas !"));
		}
	});

	RemoteConfigParamsFrame::createUi();
	logInBtn->setText(tr("Ajouter"));

}

void LocalRemoteConfigParamsFrame::addRemote()
{
	if (QFileInfo::exists(m_path))
	{
		Settings::addLocalRemote(
			{m_path.toStdString(), RemoteType::LocalHardDrive, m_remoteName->text().toStdString()});
		emit remoteAdded();
		QMessageBox::information(this, tr("Ajout d’un nouveau remote local"),
								 tr("Le remote %1 a été ajouté avec succès !").QString::arg(m_remoteName->text()));
		clearAllFields();

	} else
	{
		messageLabel->setText(tr("Le chemin n’existe pas !"));
		messageLabel->show();
	}
}
