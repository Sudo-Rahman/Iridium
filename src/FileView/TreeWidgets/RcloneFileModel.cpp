//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"
#include <QVBoxLayout>
#include <ProgressBar.hpp>


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
	container->setContentsMargins(0, 0, 0, 0);
	auto *layout = new QHBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);
	ProgressBar *progressBar;
	// change size of item if it's a double click
	if (m_expandOrDoubleClick)
	{
		progressBar = new ProgressBar(ProgressBar::Circular, container);
		progressBar->setFixedSize(40, 40);
	} else
	{
		progressBar = new ProgressBar(ProgressBar::Linear, container);
		progressBar->setFixedSize(100,15);
	}
//	// align the progress bar to the left
	layout->setAlignment(Qt::AlignLeft);
	layout->addWidget(progressBar);
	progressBar->setRange(0, 0);
	m_view->setIndexWidget(index, container);
}

RcloneFileModel::RcloneFileModel()
{
    setColumnCount(4);
    setRowCount(0);

    setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}
