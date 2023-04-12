//
// Created by sr-71 on 12/04/2023.
//

#include <QLabel>
#include <QStandardItemModel>
#include <QHeaderView>
#include "TaskTreeView.hpp"

TaskTreeView::TaskTreeView(QWidget *parent) : QTreeView(parent)
{
	setAlternatingRowColors(true);
	setUniformRowHeights(true);

	auto model = new QStandardItemModel(0, 9, this);
	QTreeView::setModel(model);

	setHeaderHidden(false);
	setRootIsDecorated(false);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	model->setHorizontalHeaderLabels(
		{tr("Source"), tr("Destination"), tr("Taille"), tr("Progression"),
		 tr("Statut"), tr("Temps restant"),
		 tr("Temps écoulé"), tr("Vitesse"), tr("Vitesse moyenne")});

}
