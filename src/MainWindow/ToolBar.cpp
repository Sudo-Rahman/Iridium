//
// Created by sr-71 on 05/04/2023.
//

#include "ToolBar.hpp"
#include <Remote.h>
#include <Settings.hpp>
#include <QStyle>

ToolBar::ToolBar(QWidget *parent) : QToolBar(parent)
{
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setMaximumHeight(30);
	setOrientation(Qt::Vertical);

	m_hideListeRemotes = new QAction(Settings::HARDDRIVE_ICON,
									 tr("Afficher les remotes"), this);
	m_hideListeRemotes->setCheckable(true);
	m_hideListeRemotes->setChecked(false);
	addAction(m_hideListeRemotes);

	connectSignals();
}

void ToolBar::connectSignals()
{
	connect(m_hideListeRemotes, &QAction::toggled, this, [this]()
	{
		m_isHideListeRemotes = !m_isHideListeRemotes;
		emit hideListeRemotes(m_isHideListeRemotes);
	});
}
