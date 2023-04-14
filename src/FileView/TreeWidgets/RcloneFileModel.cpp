//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"
#include <QVBoxLayout>
#include <QProgressBar>


/**
 * @brief RcloneFileModel::RcloneFileModel
 * @param remoteInfo
 * @param View
 */
RcloneFileModel::RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View) : m_view(View)
{
	m_remoteInfo = remoteInfo;

	setColumnCount(4);
	setRowCount(0);

	setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}

/**
 * @brief return a list of QStandardItem that have the same parent and the same file
 * @param item
 * @return list of QStandardItem
 */
QList<QStandardItem *> RcloneFileModel::getItemList(TreeFileItem *item)
{
	return {
		item,
		new TreeFileItem(item->getFile()->getSizeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getModTimeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getFileType(), item->getFile(), item)
	};
}

const QModelIndex &RcloneFileModel::getRootIndex() const
{
	return m_root_index;
}

/**
 * @brief Add a progress bar during the loading of the directory
 * @param index
 */
void RcloneFileModel::addProgressBar(const QModelIndex &index)
{
	auto *container = new QWidget;
	auto *layout = new QHBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignLeft);
	auto progressBar = new QProgressBar;
	progressBar->setMaximumWidth(100);
	layout->addWidget(progressBar);
	progressBar->setRange(0, 0);
	m_view->setIndexWidget(index, container);
}