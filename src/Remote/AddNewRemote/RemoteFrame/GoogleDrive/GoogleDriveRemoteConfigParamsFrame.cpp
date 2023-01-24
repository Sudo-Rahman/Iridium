//
// Created by rahman on 13/01/23.
//

#include <QMessageBox>
#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
	: RemoteConfigParamsFrame(parent)
{
	rclone = new Rclone;
	rclone->finished.connect([this](int exit)
							 {
								 if (exit)
								 {
									 QMessageBox::information(this, tr("Succès"),
															  tr("Le disque %1 a été ajouté avec succès").arg(
																  remoteName->text()));
								 }
							 });

	rclone->listRemotesFinished.connect([this](QMap<QString, QString> map)
										{
											listRemotes = std::move(map);
										});
	createUi();

}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{

	if (remoteName->text().isEmpty())
	{
		remoteName->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		messageLabel->show();
		messageLabel->setText(tr("Les champs en rouge sont obligatoires !"));
		return;
	}
	rclone->listRemotes();
	rclone->waitForFinished();
	if (listRemotes.contains(remoteName->text()))
	{
		QMessageBox::warning(this, tr("Erreur"), tr("Un disk du même nom existe deja"));
		remoteName->setText("");
		return;
	}
	if (rclone->getState() not_eq Rclone::Running)
	{
		rclone->config(RemoteType::Drive, {remoteName->text().toStdString()});
		rclone->waitForStarted();
		cancelBtn->show();
	}
}

void GoogleDriveRemoteConfigParamsFrame::createUi()
{

	RemoteConfigParamsFrame::createUi();

}
