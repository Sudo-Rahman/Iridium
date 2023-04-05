//
// Created by sr-71 on 05/04/2023.
//

#include "ToolBar.hpp"
#include "../Remote/Remote.h"
#include <QStyle>

ToolBar::ToolBar(QWidget *parent) : QToolBar(parent)
{
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setMaximumHeight(30);
	setOrientation(Qt::Vertical);

	m_hideListeRemotes = new QAction(QIcon::fromTheme(QString::fromStdString(HARDDRIVEICON)),
									 tr("Afficher les remotes"), this);
	m_hideListeRemotes->setCheckable(true);
	m_hideListeRemotes->setChecked(false);
	addAction(m_hideListeRemotes);

	connectSignals();
}

void ToolBar::connectSignals()
{
	connect(m_hideListeRemotes, &QAction::toggled, this, [=]()
	{
		m_isHideListeRemotes = !m_isHideListeRemotes;
		emit hideListeRemotes(m_isHideListeRemotes);
	});
}
