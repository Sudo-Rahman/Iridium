//
// Created by rahman on 13/01/23.
//

#include <QMessageBox>
#include "GoogleDriveRemoteConfigParamsFrame.hpp"

GoogleDriveRemoteConfigParamsFrame::GoogleDriveRemoteConfigParamsFrame(QWidget *parent)
        : RemoteConfigParamsFrame(parent)
{
    rclone = new Rclone;
    connect(rclone, &Rclone::exitCode, this, [this](int exit)
    {
        if (exit)
        {
            QMessageBox::information(this, tr("Succès"),
                                     tr("Le disque %1 a été ajouté avec succès").arg(remoteName->text()));
        }
    });
    connect(rclone, &Rclone::listRemotesFinished, this, [this](QMap<QString, QString> map)
    {
        listRemotes = map;
    });
    createUi();

}

void GoogleDriveRemoteConfigParamsFrame::addRemote()
{
    rclone->listRemotes();
    rclone->waitForFinished();
    if (listRemotes.contains(remoteName->text()))
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Un disk du même nom existe deja"));
        remoteName->setText("");
        return;
    }
    if (rclone not_eq nullptr and rclone->state() == Rclone::Running)
    {
        rclone->terminate();
        rclone->waitForFinished(10);
    }
    rclone->config(RemoteType::Drive, {remoteName->text()});
}

void GoogleDriveRemoteConfigParamsFrame::createUi()
{

    RemoteConfigParamsFrame::createUi();

}
