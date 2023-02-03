//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>

#include "TreeFileView.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <thread>
#include <QMessageBox>

TreeFileView::TreeFileView(QString remoteName, QWidget *parent) : remoteName(std::move(remoteName)),
																  QTreeView(parent)
{
	setAlternatingRowColors(true);
	header()->setSectionsMovable(true);
	header()->resizeSection(0, 300);
	header()->setStretchLastSection(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	model = new RcloneFileModel(RcloneFileModel::Distant, TreeFileView::remoteName);
	QTreeView::setModel(model);

	connect(this, &QTreeView::doubleClicked, this, [=](const QModelIndex &index)
	{
		auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
		dynamic_cast<RcloneFileModel *>(model)->addItemLocal(item->getFile()->getPath(), item);
		auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
		indexBack.push_back(id);
		QTreeView::setRootIndex(item->getParent() == nullptr ? index : model->indexFromItem(item->getParent()));
	});

	connect(this, &QTreeView::expanded, this, [=](const QModelIndex &index)
	{
		auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
		dynamic_cast<RcloneFileModel *>(model)->addItemLocal(item->getFile()->getPath(), item);

	});


}

const QString &TreeFileView::getRemoteName() const
{
	return remoteName;
}


void TreeFileView::resizeEvent(QResizeEvent *event)
{
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	header()->setSectionResizeMode(0, QHeaderView::Interactive);

}

void TreeFileView::back()
{
	if (indexBack.length() > 0)
	{
		QTreeView::setRootIndex(indexBack.back());
		indexBack.pop_back();
	} else
	{
		QTreeView::setRootIndex(dynamic_cast<RcloneFileModel *>(model)->getRootIndex());
	}
}

