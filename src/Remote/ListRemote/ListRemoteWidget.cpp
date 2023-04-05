//
// Created by sr-71 on 29/03/2023.
//

#include "ListRemoteWidget.hpp"
#include "../AddNewRemote/AddNewRemoteDialog.hpp"

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
	m_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
	m_layout->setSpacing(10);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	// padding
	m_layout->setContentsMargins(10, 10, 15, 10);

	auto *toplayout = new QHBoxLayout;

	m_add = new QPushButton("+");
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
		m_listRemote.append(remote);
		m_remoteLayout->addWidget(remote);
		connect(remote, &RemoteWidget::clicked, this, [this, remote]()
		{
			emit remoteClicked(remote);
		});
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