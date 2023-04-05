//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"

#include <utility>

RcloneFileModel::RcloneFileModel(const RemoteInfoPtr &remoteInfo, QObject *parent) : QStandardItemModel(
	parent)
{
	m_remoteInfo = remoteInfo;

	setColumnCount(4);
	setRowCount(0);


	setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}

QList<QStandardItem *> RcloneFileModel::getItemList(TreeFileItem *item)
{
	return {
		item,
		new TreeFileItem(item->getFile()->getSizeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getModTimeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getIsDirString(), item->getFile(), item)
	};
}

const QModelIndex &RcloneFileModel::getRootIndex() const
{
	return m_root_index;
}
