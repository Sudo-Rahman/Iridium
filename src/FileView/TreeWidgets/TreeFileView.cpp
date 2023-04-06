//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>
#include <QMouseEvent>
#include <QMenu>
#include <QApplication>
#include "TreeFileView.hpp"
#include "RcloneFileModelDistant.hpp"
#include "RcloneFileModelLocal.hpp"
#include "ItemMenu.hpp"
#include "RcloneProxy.hpp"
#include "ItemInfoDialog.hpp"
#include <QEvent>


TreeFileView::TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent) : QTreeView(parent)
{
	m_remoteInfo = remoteInfo;
	setIndentation(15);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
//	setFocusPolicy(Qt::NoFocus);
	setIconSize(QSize(24, 24));
	setAlternatingRowColors(true);
	header()->setSectionsMovable(true);
	header()->setFont(QFont("Arial", 13, QFont::DemiBold));
	setUniformRowHeights(true);
	header()->setSectionsClickable(true);
	header()->setSortIndicatorShown(true);
	header()->setStretchLastSection(false);


	//set focus only window
	setFocusPolicy(Qt::StrongFocus);

	RcloneFileModel *rcloneFileModel;
	if (!m_remoteInfo->isLocal())
		rcloneFileModel = new RcloneFileModelDistant(m_remoteInfo, RcloneFileModelDistant::Dynamic);
	else
		rcloneFileModel = new RcloneFileModelLocal(m_remoteInfo);

	TreeFileView::setModel(rcloneFileModel);

	auto *proxy = new RcloneProxy(this);
	proxy->setSourceModel(rcloneFileModel);

	//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	header()->setSectionResizeMode(2, QHeaderView::Interactive);
	header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

	connectSignals();

}

void TreeFileView::connectSignals()
{
	connect(header(), &QHeaderView::sortIndicatorChanged, this, [=](int logicalIndex, Qt::SortOrder order)
	{
		sortByColumn(logicalIndex, order);
	});

	connect(this, &QTreeView::doubleClicked, this, &TreeFileView::doubleClick);

	connect(this, &QTreeView::expanded, this, &TreeFileView::expand);

	connect(this, &QTreeView::collapsed, [=](const QModelIndex &index)
	{
		// collapse all children
		for (int i = 0; i < model->rowCount(index); i++)
		{
			auto child = model->index(i, 0, index);
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
	emit pathChanged(getPath());
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
	emit pathChanged(getPath());
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
	if (!item->getFile()->isDir())
		return;
	auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
	indexBack.push_back(id);
	indexTop.clear();
	QTreeView::setRootIndex(item->getParent() == nullptr ? index : model->indexFromItem(item->getParent()));
	emit pathChanged(getPath());
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
		auto lisItem = getSelectedItems();

		connect(&menu, &ItemMenu::info, this, [&menu, lisItem, this]()
		{
			for (auto item: lisItem)
			{
				auto *dialog = new ItemInfoDialog(item, this);
				dialog->move(QPoint(rand() % 1000, rand() % 1000));
				dialog->show();
			}
		});

		connect(&menu, &ItemMenu::copyed, this, [this, lisItem]()
		{
			qDebug() << lisItem.first()->getFile()->getName();
			emit fileCopied(lisItem.first());
		});
		connect(&menu, &ItemMenu::pasted, this, [this, lisItem]()
		{
			qDebug() << lisItem.first()->getFile()->getPath();
			emit pasted(lisItem.first()->getFile());
		});
		connect(&menu, &ItemMenu::deleted, this, [this, lisItem]()
		{
			QList<TreeFileItem *> lst;
			for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
			{
				auto index = QTreeView::selectedIndexes().at(i);
				auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(
					index)));
				lst << item;
				qDebug() << item->getFile()->getPath();
			}
			deleteFile(lst);
		});
		menu.exec(event->globalPosition().toPoint());
	}
}

void TreeFileView::changeRemote(const RemoteInfoPtr &info)
{
	m_remoteInfo = info;
	delete model;
	if (!m_remoteInfo->isLocal())
		model = new RcloneFileModelDistant(m_remoteInfo, RcloneFileModelDistant::Dynamic);
	else
		model = new RcloneFileModelLocal(m_remoteInfo);
	QTreeView::setModel(model);
}

// get path of selected item
QString TreeFileView::getPath()
{
	auto index = QTreeView::rootIndex();
	if (!index.isValid())
		return "";
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	return item->getFile()->getPath();
}

void TreeFileView::paste(TreeFileItem *item)
{
	if (item == nullptr)
		return;
	auto treePaste = getSelectedItems().first();
	if (!treePaste->getFile()->isDir())
		return;
	RcloneFilePtr newFile = std::make_shared<RcloneFile>(
		treePaste->getFile()->getPath() + "/" + item->getFile()->getName(),
		item->getFile()->getSize(),
		item->getFile()->isDir(),
		item->getFile()->getModTime(),
		m_remoteInfo
	);
	auto *treeItem = new TreeFileItem(newFile->getName(), newFile, treePaste, true);
	auto list = RcloneFileModel::getItemList(treeItem);
	treePaste->appendRow(list);
}

QList<TreeFileItem *> TreeFileView::getSelectedItems()
{
	QList<TreeFileItem *> lst;
	for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
	{
		auto index = QTreeView::selectedIndexes().at(i);
		auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
		lst << item;
	}
	if (lst.isEmpty())
	{
		auto index = QTreeView::rootIndex();
		if (index.isValid())
		{
			auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
			lst << item;
		} else
		{
			index = model->index(0, 0, QTreeView::rootIndex());
			auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
			lst << item;
		}
	}
	return lst;
}

void TreeFileView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Space:
		{
			for (auto item: getSelectedItems())
			{
				auto *info = new ItemInfoDialog(item, this);
				// show in random position
				info->move(QPoint(rand() % 1000, rand() % 1000));
				info->show();
			}
		}
			break;
		case Qt::Key_C:
		{
			emit fileCopied(getSelectedItems().first());
		}
			break;
		case Qt::Key_V:
			emit pasted(getSelectedItems().first()->getFile());
			break;
		case Qt::Key_Delete:
			deleteFile(
				[=]()
				{
					QList<TreeFileItem *> lst;
					for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
					{
						auto index = QTreeView::selectedIndexes().at(i);
						auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(
							index)));
						lst << item;
					}
					return lst;
				}()
			);
			break;
		case Qt::Key_Left:
			back();
			break;
		case Qt::Key_Right:
			front();
			break;
		default:
			QTreeView::keyPressEvent(event);
	}
}

void TreeFileView::deleteFile(const QList<TreeFileItem *> &items)
{
	auto files = [items]() -> auto
	{
		QStringList lst;
		for (auto item: items)
			lst << item->getFile()->getName();
		return lst;
	}();

	for (auto item: items)
	{
		auto rclone = m_rclonesManager.get();
		connect(rclone.get(), &Rclone::fileDeleted, this, [this, files, item, rclone](const int exit)
		{
			if (exit == 0)
			{
				m_rclonesManager.release(rclone);
//				QMessageBox::information(this, tr("Suppression"), files.join(", ") + tr(" Supprimé"));
				removeItem(item);
			} else
			{
				QMessageBox::critical(this, tr("Suppression"), files.join(", ") + tr(" non supprimé"));
			}
		});
		rclone->deleteFile(item->getFile().operator*());
	}
}

void TreeFileView::removeItem(TreeFileItem *item)
{
	// remove recursively
	if (item->getFile()->isDir())
	{
		for (int i = 0; i < item->rowCount(); i++)
		{
			auto *child = dynamic_cast<TreeFileItem *>(item->child(i));
			if (child not_eq nullptr)
				removeItem(child);
		}
	}
	model->removeRow(item->row(), model->indexFromItem(item).parent());
}
