//
// Created by rahman on 13/01/23.
//

#include <QPainter>
#include "RemoteConfigParamsFrame.hpp"

using namespace std;

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{
	layout = new QVBoxLayout(this);

	auto *tmpLayout = new QHBoxLayout;
	tmpLayout->addWidget(new QLabel(tr("Nom : "), this));

	remoteName = new QLineEdit(this);
	remoteName->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
	connect(remoteName, &QLineEdit::textChanged, this, [=](const QString &text)
	{
		if (not text.isEmpty())
			remoteName->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
		if (not messageLabel->isHidden())
			messageLabel->hide();
	});
	tmpLayout->addWidget(remoteName);

	layout->addLayout(tmpLayout);


}

void RemoteConfigParamsFrame::createUi()
{
	auto *tmpwidlay = new QHBoxLayout;

	logInBtn = new QPushButton(tr("Se connecter"), this);
	logInBtn->setDefault(true);
	logInBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	tmpwidlay->addWidget(logInBtn, Qt::AlignTop);
	tmpwidlay->setAlignment(logInBtn, Qt::AlignTop);

	cancelBtn = new QPushButton(tr("Annuler"), this);
	cancelBtn->hide();
	cancelBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	tmpwidlay->addWidget(cancelBtn, Qt::AlignTop);
	tmpwidlay->setAlignment(cancelBtn, Qt::AlignTop);

	layout->addLayout(tmpwidlay);

	connect(logInBtn, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);

	connect(cancelBtn, &QPushButton::clicked, this, [=]()
	{
		if (rclone->getState() == Rclone::Running)
			rclone->terminate();
		cancelBtn->hide();
	});

	messageLabel = new QLabel(this);
	messageLabel->hide();
	messageLabel->setAutoFillBackground(true);
	QPalette p;
	p.setColor(QPalette::WindowText, Qt::red);
	messageLabel->setPalette(p);
	layout->addWidget(messageLabel, Qt::AlignTop);
	layout->setAlignment(messageLabel, Qt::AlignTop);
}

void RemoteConfigParamsFrame::addRemote()
{
	if (remoteName->text().isEmpty())
	{
		remoteName->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		messageLabel->show();
		messageLabel->setText(tr("Les champs en rouge sont obligatoires !"));
		return;
	}

	Rclone rclone_liste_remote;
	rclone_liste_remote.listRemotesFinished.connect([this](map<string , string> map)
													{
														listRemotes = std::move(map);
													});
	rclone_liste_remote.listRemotes();
	rclone_liste_remote.waitForStarted();
	rclone_liste_remote.waitForFinished();
}
