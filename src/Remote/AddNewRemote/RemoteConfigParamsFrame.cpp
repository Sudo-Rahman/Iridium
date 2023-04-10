//
// Created by rahman on 13/01/23.
//

#include <QPainter>
#include <QMessageBox>

#include "RemoteConfigParamsFrame.hpp"

using namespace std;

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{
	m_layout = new QVBoxLayout(this);

	auto *tmpLayout = new QHBoxLayout;
	tmpLayout->addWidget(new QLabel(tr("Nom : "), this));

	m_remoteName = new QLineEdit(this);
	m_remoteName->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
	connect(m_remoteName, &QLineEdit::textChanged, this, [this](const QString &text)
	{
		if (not text.isEmpty())
			m_remoteName->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
		if (not messageLabel->isHidden())
			messageLabel->hide();
	});
	tmpLayout->addWidget(m_remoteName);

	m_layout->addLayout(tmpLayout);

	m_rclone = Rclone::instance();
	connect(m_rclone.get(), &Rclone::configFinished, this, [this](int exit)
	{
		if (exit == 0)
		{
			emit remoteAdded();
			QMessageBox::information(this, tr("Succès"),
									 tr("Le disque %1 a été ajouté avec succès").arg(m_remoteName->text()));
			clearAllFields();
		} else
		{
			auto *msgBox = new QMessageBox();
			msgBox->setWindowTitle(tr("Erreur"));
			msgBox->setText(tr("Une erreur est survenue lors de la configuration du serveur distant !"));
			msgBox->setDetailedText(m_rclone->readAllError().c_str());
			msgBox->setStandardButtons(QMessageBox::Ok);
			msgBox->exec();
		}
	});
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

	m_layout->addLayout(tmpwidlay);

	connect(logInBtn, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);

	connect(cancelBtn, &QPushButton::clicked, this, [this]()
	{
		if (m_rclone->getState() == Rclone::Running)
			m_rclone->terminate();
		cancelBtn->hide();
		logInBtn->show();
	});

	messageLabel = new QLabel(this);
	messageLabel->hide();
	messageLabel->setAutoFillBackground(true);
	QPalette p;
	p.setColor(QPalette::WindowText, Qt::red);
	messageLabel->setPalette(p);
	m_layout->addWidget(messageLabel, Qt::AlignTop);
	m_layout->setAlignment(messageLabel, Qt::AlignTop);
}

void RemoteConfigParamsFrame::addRemote()
{
	if (m_remoteName->text().isEmpty())
	{
		m_remoteName->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		messageLabel->show();
		messageLabel->setText(tr("Les champs en rouge sont obligatoires !"));
		return;
	}

	Rclone rclone_liste_remote;
	rclone_liste_remote.listRemotes();
	rclone_liste_remote.waitForStarted();
	rclone_liste_remote.waitForFinished();
	listRemotes = rclone_liste_remote.getData();

}

bool RemoteConfigParamsFrame::checkFields()
{
	if (m_remoteName->text().isEmpty())
	{
		m_remoteName->setStyleSheet("border: 1px solid red; border-radius: 5px;");
		messageLabel->show();
		messageLabel->setText(tr("Les champs en rouge sont obligatoires !"));
		return false;
	}
	return true;
}

void RemoteConfigParamsFrame::connecLineEdit(QLineEdit *lineEdit)
{
	lineEdit->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
	connect(lineEdit, &QLineEdit::textChanged, this, [=](const QString &text)
	{
		if (not text.isEmpty())
			lineEdit->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
		else
			lineEdit->setStyleSheet("border: 1px solid red; border-radius: 5px;");
	});
}

void RemoteConfigParamsFrame::clearAllFields()
{
	for (auto &field: findChildren<QLineEdit *>())
	{
		field->clear();
		field->setStyleSheet("border: 1px solid gray; border-radius: 5px;");
	}
}
