//
// Created by rahman on 05/02/23.
//

#include "RcloneFileModelDistant.hpp"

#include <utility>

RcloneFileModelDistant::RcloneFileModelDistant(QString path, Load load, QObject *parent) : RcloneFileModel(
	std::move(path),
	parent), load(load)
{
	RcloneFileModelDistant::init();
}

void RcloneFileModelDistant::init()
{
	auto *drive = new TreeFileItem(path);
	drive->setIcon(QIcon::fromTheme(toQString(HARDDRIVEICON)));
	m_root_index = drive->index();
	if (load == Dynamic)
		drive->appendRow({});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive),
			  });
	if (load == Static)
		initStatic(path, drive);
}

void RcloneFileModelDistant::addItem(const QString &path, TreeFileItem *parent)
{
	if (load == Dynamic)
	{
		addItemDynamic(path, parent);
	}
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItem(path, file.toObject());
			tree_item->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				item->appendRow({});
		}
		if (not doc.array().isEmpty())
			tree_item->removeRow(0);
	});
	if (tree_item->rowCount() == 1)
		rclone->lsJson(path.toStdString());
}

void RcloneFileModelDistant::initStatic(const QString &path, TreeFileItem *parent)
{
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItem(path, file.toObject());
			parent->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				initStatic(item->getFile()->getPath(), item);
		}
	});
	rclone->lsJson(path.toStdString());
}

