//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"


RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, Load load, QObject *parent)
	: RcloneFileModel(
	remoteInfo,
	parent), load(load)
{
	RcloneFileModelDistant::init();
	if (load == Dynamic)
		m_timer.setInterval(125);
}

void RcloneFileModelDistant::init()
{
	auto *drive = new TreeFileItem(QString::fromStdString(m_remoteInfo->m_path), m_remoteInfo);
	drive->setIcon(QIcon(QString::fromStdString(m_remoteInfo->m_icon)));
	m_root_index = drive->index();
	if (load == Dynamic)
		drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive),
			  });
	if (load == Static)
		initStatic(QString::fromStdString(m_remoteInfo->m_path), drive);
}

void RcloneFileModelDistant::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	if (load == Dynamic)
		addItemDynamic(file->getPath(), parent);
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{

	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [tree_item, path, this](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItemDistant(path, m_remoteInfo, file.toObject());
			tree_item->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				item->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
		}
		delete tree_item->child(0, 0);
		tree_item->removeRow(0);
	});
	if (tree_item->rowCount() == 1)
		rclone->lsJson(path.toStdString());
}

void RcloneFileModelDistant::initStatic(const QString &path, TreeFileItem *parent)
{
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [path, parent, this](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItemDistant(path, m_remoteInfo, file.toObject());
			parent->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				initStatic(item->getFile()->getPath(), item);
		}
	});
	rclone->lsJson(path.toStdString());
}
