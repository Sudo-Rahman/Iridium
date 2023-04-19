//
// Created by sr-71 on 11/02/2023.
//

#include <QDialog>
#include <QLayout>
#include "ItemMenu.hpp"
#include "ItemInfoDialog.hpp"

ItemMenu::ItemMenu(QWidget *parent) : QMenu(parent)
{
	m_info = new QAction(tr("Information"), this);
	m_info->setShortcut(QKeySequence::fromString("space"));
	m_copy = new QAction(tr("Copier"), this);
	m_paste = new QAction(tr("Coller"), this);
	m_delete = new QAction(tr("Supprimer"), this);
	m_newFolder = new QAction(tr("Nouveau dossier"), this);
	addAction(m_info);
	addAction(m_copy);
	addAction(m_paste);
	addAction(m_delete);
	addAction(m_newFolder);


	connect(m_copy, &QAction::triggered, this, &ItemMenu::copyed);
	connect(m_paste, &QAction::triggered, this, &ItemMenu::pasted);
	connect(m_info, &QAction::triggered, this, &ItemMenu::info);
	connect(m_delete, &QAction::triggered, this, &ItemMenu::deleted);
	connect(m_newFolder, &QAction::triggered, this, &ItemMenu::newFolder);

}


void ItemMenu::setActionEnabled(const QList<QPair<Action, bool>> &lst)
{
	for (const auto &pair: lst)
	{
		switch (pair.first)
		{
			case Copy:
				m_copy->setEnabled(pair.second);
				break;
			case Paste:
				m_paste->setEnabled(pair.second);
				break;
			case Info:
				m_info->setEnabled(pair.second);
				break;
			case Delete:
				m_delete->setEnabled(pair.second);
				break;
			case NewFolder:
				m_newFolder->setEnabled(pair.second);
				break;
		}
	}

}
