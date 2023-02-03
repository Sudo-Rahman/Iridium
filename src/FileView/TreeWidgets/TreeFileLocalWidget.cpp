//
// Created by sr-71 on 29/01/2023.
//

#include "TreeFileLocalWidget.hpp"
#include <QModelIndex>
#include <QString>
#include <boost/process.hpp>
#include <QMimeDatabase>


TreeFileLocalWidget::TreeFileLocalWidget(QWidget *parent) : QTreeWidget(parent)
{
	setHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});

	setAlternatingRowColors(true);

	auto root = QFileInfo("/Users/sr-71/Downloads/Fluent-icon-theme-master.zip");
	qDebug() << QMimeDatabase().mimeTypeForFile(root).iconName();
//	for(const auto &l : root.entryInfoList()){
//		qDebug() << QMimeDatabase().mimeTypeForFile(l.path()).iconName();
//		qDebug() << QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(l.path()).iconName());
//	}

	connect(this, &QTreeView::doubleClicked, this, [=](const QModelIndex &index)
	{});

}
