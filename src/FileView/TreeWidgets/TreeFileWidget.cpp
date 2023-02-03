//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>

#include "TreeFileWidget.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <thread>
#include <QMessageBox>

TreeFileWidget::TreeFileWidget(QString remoteName, QWidget *parent) : remoteName(std::move(remoteName)),
																	  QTreeView(parent)
{
	setAlternatingRowColors(true);
	header()->setSectionsMovable(true);
	header()->resizeSection(0, 300);
	header()->setStretchLastSection(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	model = new RcloneFileModel(RcloneFileModel::Local, TreeFileWidget::remoteName);
	QTreeView::setModel(model);

	connect(this, &QTreeView::doubleClicked, this, [=](const QModelIndex &index)
	{
		auto i = static_cast<TreeFileItem *>(model->itemFromIndex(index));
//		moItemLocal(i->getFile()->getPath(),i);
qDebug()  << i->getFile()->getPath();
		reinterpret_cast<RcloneFileModel *>(model)->addItemLocal(i->getFile()->getPath(),i);
		indexBack.push_back(indexAbove(currentIndex()));
		QTreeView::setRootIndex(index);
	});


}

const QString &TreeFileWidget::getRemoteName() const
{
	return remoteName;
}


void TreeFileWidget::resizeEvent(QResizeEvent *event)
{
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	header()->setSectionResizeMode(0, QHeaderView::Interactive);

}

void TreeFileWidget::back()
{
	if (indexBack.length() > 0)
	{
		QTreeView::setRootIndex(indexBack.back());
		indexBack.pop_back();
	}
}

