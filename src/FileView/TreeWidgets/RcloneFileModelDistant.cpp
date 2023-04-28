//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"

uint8_t RcloneFileModelDistant::m_maxDepth = 2;
RcloneFileModelDistant::Load RcloneFileModelDistant::m_load = Dynamic;

RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *View)
	: RcloneFileModel(remoteInfo, View)
{
	RcloneFileModelDistant::init();
}

void RcloneFileModelDistant::init()
{
	auto *drive = new TreeFileItem(QString::fromStdString(m_remoteInfo->m_path), m_remoteInfo);
	drive->setIcon(QIcon(QString::fromStdString(m_remoteInfo->m_icon)));
	m_root_index = drive->index();
	drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive),
			  });
	if (m_load == Static)
		addItemStatic(QString::fromStdString(m_remoteInfo->m_path), drive);
}

void RcloneFileModelDistant::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	if (m_load == Dynamic)
		addItemDynamic(file->getPath(), parent);
	else
		addItemStatic(file->getPath(), parent);
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	auto rclone = RcloneManager::get();
	if (tree_item->state() == TreeFileItem::NotLoaded)
	{
		addProgressBar(tree_item->child(0, 0)->index());
		tree_item->setState(TreeFileItem::Loading);
		connect(rclone.get(), &Rclone::lsJsonFinished, this, [tree_item, path, this](const QJsonDocument &doc)
		{
			tree_item->removeRow(0);
			for (const auto &file: doc.array())
			{
				auto *item = new TreeFileItemDistant(path, m_remoteInfo, file.toObject());
				tree_item->appendRow(getItemList(item));
				if (item->getFile()->isDir())
					item->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
			}
			tree_item->setState(TreeFileItem::Loaded);
		});
		rclone->lsJson(path.toStdString());
	}
}

void RcloneFileModelDistant::addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth)
{
	// TODO: not working correctly
	if (depth == 0)
		return;
//	if (depth == m_maxDepth)
//	{
//		for (auto &it: m_rcloneList)
//		{
//			if (it->state() == Rclone::NotLaunched)
//			{
//				it->terminate();
////				RcloneManager::release(it);
//			}
//		}
//		m_rcloneList.clear();
//	}

	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	auto rclone = RcloneManager::get();
	m_rcloneList.push_back(rclone);
	if ( tree_item->state() == TreeFileItem::NotLoaded)
	{
		addProgressBar(tree_item->child(0, 0)->index());
		connect(rclone.get(), &Rclone::lsJsonFinished, this,
				[depth, tree_item, path, parent, this](const QJsonDocument &doc)
				{
					tree_item->removeRow(0);
					for (const auto &file: doc.array())
					{
						auto *item = new TreeFileItemDistant(path, m_remoteInfo, file.toObject());
						parent->appendRow(getItemList(item));
						if (item->getFile()->isDir())
						{
							item->appendRow(
								{new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
							if (not m_newIndex)
								addItemStatic(item->getFile()->getPath(), item, depth - 1);
						}
					}
					tree_item->setState(TreeFileItem::Loaded);
				});
		rclone->lsJson(path.toStdString());
		tree_item->setState(TreeFileItem::Loading);
	}
}
