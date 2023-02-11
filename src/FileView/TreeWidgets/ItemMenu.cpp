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
	m_copy = new QAction(tr("Copier"), this);
	m_paste = new QAction(tr("Coller"), this);
	addAction(m_info);
	addAction(m_copy);
	addAction(m_paste);

	connect(m_copy, &QAction::triggered, this, &ItemMenu::copyed);
	connect(m_paste, &QAction::triggered, this, &ItemMenu::pasted);
	connect(m_info, &QAction::triggered, this, &ItemMenu::info);

}

void ItemMenu::m_info_click(TreeFileItem *file)
{
	ItemInfoDialog dialog(file, this);
	dialog.exec();

}
