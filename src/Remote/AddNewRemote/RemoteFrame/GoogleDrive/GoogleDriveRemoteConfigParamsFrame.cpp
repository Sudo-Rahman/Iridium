//
// Created by rahman on 13/01/23.
//

#include <QMessageBox>
#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
	: RemoteConfigParamsFrame(parent)
{
	rclone = new Rclone;
	connect(rclone, &Rclone::configFinished, this, [=](int exit)
	{
		if (exit == 0)
		{
			QMessageBox::information(this, tr("Succès"),
									 tr("Le disque %1 a été ajouté avec succès").arg(
										 remoteName->text()));
			remoteName->setText("");
			cancelBtn->hide();
		}
	});

	createUi();

}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{

	RemoteConfigParamsFrame::addRemote();
	if (remoteName->text().isEmpty())
		return;
	if (listRemotes.contains(remoteName->text()))
	{
		QMessageBox::warning(this, tr("Erreur"), tr("Un disk du même nom existe deja"));
		remoteName->setText("");
		return;
	}
	rclone->config(RemoteType::Drive, {remoteName->text().toStdString()});
	rclone->waitForStarted();
	cancelBtn->show();

}

void GoogleDriveRemoteConfigParamsFrame::createUi()
{

	RemoteConfigParamsFrame::createUi();

}
