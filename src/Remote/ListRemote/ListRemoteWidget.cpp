//
// Created by sr-71 on 29/03/2023.
//

#include <QPropertyAnimation>
#include "ListRemoteWidget.hpp"
#include "../AddNewRemote/AddNewRemoteDialog.hpp"
#include "../../Config/Settings.hpp"

/**
 * @brief constructeur
 * @param parent
 */
ListRemoteWidget::ListRemoteWidget(QWidget *parent) : QScrollArea(parent)
{
	setWidgetResizable(true);
	auto *widget = new QWidget(this);
	setWidget(widget);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	m_layout = new QVBoxLayout(widget);
	m_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	m_layout->setSpacing(10);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	// padding
	m_layout->setContentsMargins(10, 10, 15, 10);

	auto *toplayout = new QHBoxLayout;

	m_add = new RoundedButton("+");
	m_add->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(m_add, &QPushButton::clicked, this, [this]()
	{
		auto *addRemote = new AddNewRemoteDialog(this);
		addRemote->exec();
	});
	toplayout->addWidget(m_add);

	m_recherche = new QLineEdit;
	m_recherche->setPlaceholderText(tr("Rechercher un remote"));
	connect(m_recherche, &QLineEdit::textChanged, this, &ListRemoteWidget::searchRemote);
	toplayout->addWidget(m_recherche);

	m_layout->addLayout(toplayout);

	m_remoteLayout = new QVBoxLayout;
	m_layout->addLayout(m_remoteLayout);


	getAllRemote();

	// no border
	setFrameShape(QFrame::NoFrame);

	m_width = QScrollArea::sizeHint().width();

	setMaximumWidth(0);
}

/**
 * @brief Récupère tous les remotes
 */
void ListRemoteWidget::getAllRemote()
{
	m_listRemote.clear();
	auto rclone = m_manager.get();
	rclone->listRemotes();
	rclone->waitForFinished();
	for (const auto &pair: rclone->getData())
	{
		auto *remote = new RemoteWidget({pair.first, stringToRemoteType.find(pair.second)->second,
										}, this);
		m_listRemote << remote;
	}

	// ajout du local
	auto *local = new RemoteWidget({"/", RemoteType::LocalHardDrive, "local"}, this);
	m_listRemote.push_front(local);

	for (const auto &localRemote: Settings::getLocalRemotes())
	{
		auto *remote = new RemoteWidget(localRemote, this);
		m_listRemote << remote;
	}


	for (auto *remote: m_listRemote)
	{
		connect(remote, &RemoteWidget::clicked, this, [this, remote]()
		{ emit remoteClicked(remote);});
		m_remoteLayout->addWidget(remote);
	}
}

/**
 * @brief Recherche un remote en fonction de son nom
 * @param name
 */
void ListRemoteWidget::searchRemote(const QString &name)
{
	for (auto *remote: m_listRemote)
	{
		if (QString::fromStdString(remote->remoteInfo()->name()).contains(name, Qt::CaseInsensitive))
			remote->show();
		else
			remote->hide();
	}
}

void ListRemoteWidget::expand()
{
	auto animation = new QPropertyAnimation(this, "maximumWidth");
	animation->setDuration(300);
	animation->setStartValue(width());
	animation->setEndValue(m_isExpand ? 0 : m_width);
	animation->setEasingCurve(QEasingCurve::InOutQuad);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	m_isExpand = !m_isExpand;
}
