//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>
#include "TreeFileView.hpp"
#include "RcloneFileModelDistant.hpp"

TreeFileView::TreeFileView(Remote type, QString remoteName, QWidget *parent) : remoteName(std::move(remoteName)),
																			   QTreeView(parent), type(type)
{
	setAlternatingRowColors(true);
	header()->setSectionsMovable(true);
	header()->resizeSection(0, 300);
	header()->setStretchLastSection(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (type == Distant)
		TreeFileView::setModel(new RcloneFileModelDistant(TreeFileView::remoteName, RcloneFileModelDistant::Dynmic));


	connect(this, &QTreeView::doubleClicked, this, &TreeFileView::doubleClick);

	connect(this, &QTreeView::expanded, this, &TreeFileView::expand);


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
		auto index = indexBack.back();
		indexBack.pop_back();
		indexTop << index.sibling(1, 0);
		QTreeView::setRootIndex(index);
	} else
		QTreeView::setRootIndex(dynamic_cast<RcloneFileModel *>(model)->getRootIndex());
}

void TreeFileView::front()
{
	if (indexTop.length() > 0)
	{
		auto index = indexTop.back();
		indexTop.pop_back();
		indexBack << index.parent();
		QTreeView::setRootIndex(index);
	}
}

void TreeFileView::expand(const QModelIndex &index)
{
	QTreeView::expand(index);
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile()->getPath(), item);
}

void TreeFileView::doubleClick(const QModelIndex &index)
{
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile()->getPath(), item);
	auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
	indexBack.push_back(id);
	QTreeView::setRootIndex(item->getParent() == nullptr ? index : model->indexFromItem(item->getParent()));
}

void TreeFileView::setModel(RcloneFileModel *model)
{
	TreeFileView::model = model;
	QTreeView::setModel(TreeFileView::model);
}

