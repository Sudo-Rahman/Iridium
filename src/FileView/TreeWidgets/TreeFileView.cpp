//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include "TreeFileView.hpp"
#include "RcloneFileModelDistant.hpp"
#include "RcloneFileModelLocal.hpp"
#include "ItemMenu.hpp"
#include "RcloneProxy.hpp"

TreeFileView::TreeFileView(RemoteInfo remoteInfo, QWidget *parent) : m_remoteInfo(std::move(remoteInfo)),
																			   QTreeView(parent)
{
	setIndentation(15);
	// recursive collapse
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setFocusPolicy(Qt::NoFocus);
	setIconSize(QSize(25, 25));
	setAlternatingRowColors(true);
	header()->setSectionsMovable(true);
	header()->setFont(QFont("Arial", 13, QFont::DemiBold));
	setUniformRowHeights(true);
	header()->setSectionsClickable(true);
	header()->setSortIndicatorShown(true);
	header()->setStretchLastSection(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	header()->resizeSection(0, 200);

	RcloneFileModel *rcloneFileModel;
	if (!m_remoteInfo.isLocal())
		rcloneFileModel = new RcloneFileModelDistant(m_remoteInfo, RcloneFileModelDistant::Dynamic);
	else
		rcloneFileModel = new RcloneFileModelLocal(m_remoteInfo);

	TreeFileView::setModel(rcloneFileModel);

	auto *proxy = new RcloneProxy(this);
	proxy->setSourceModel(rcloneFileModel);

	connect(header(), &QHeaderView::sortIndicatorChanged, this, [=](int logicalIndex, Qt::SortOrder order)
	{
		sortByColumn(logicalIndex, order);
	});

	connect(this, &QTreeView::doubleClicked, this, &TreeFileView::doubleClick);

	connect(this, &QTreeView::expanded, this, &TreeFileView::expand);

	connect(this,&QTreeView::collapsed,[=](const QModelIndex &index)
	{
		// collapse all children
		for(int i = 0; i < model->rowCount(index); i++)
		{
			auto child = model->index(i,0,index);
			collapse(child);
		}
	});


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
		indexTop << QTreeView::rootIndex();
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
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
}

void TreeFileView::doubleClick(const QModelIndex &index)
{
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
	auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
	indexBack.push_back(id);
	indexTop.clear();
	QTreeView::setRootIndex(item->getParent() == nullptr ? index : model->indexFromItem(item->getParent()));
}

void TreeFileView::setModel(RcloneFileModel *model)
{
	TreeFileView::model = model;
	QTreeView::setModel(TreeFileView::model);
}

void TreeFileView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	if (event->button() == Qt::RightButton)
	{
		ItemMenu menu(this);
		if (not QTreeView::selectedIndexes().isEmpty())
		{
			connect(&menu, &ItemMenu::info, this, [&menu, this]()
			{
				menu.m_info_click(
					reinterpret_cast<TreeFileItem *>(model->itemFromIndex(QTreeView::selectedIndexes().first())));
			});
		}
		menu.exec(event->globalPosition().toPoint());
	}
}

void TreeFileView::changeRemote(const RemoteInfo &info)
{
	m_remoteInfo = info;
	delete model;
	if (!m_remoteInfo.isLocal())
		model = new RcloneFileModelDistant(m_remoteInfo, RcloneFileModelDistant::Dynamic);
	else
		model = new RcloneFileModelLocal(m_remoteInfo);
	QTreeView::setModel(model);
}
