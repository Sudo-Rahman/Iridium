//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"

RcloneFileModel::RcloneFileModel(RcloneFileModel::Type type, const QString &path, QObject *parent) : QStandardItemModel(
	parent), path(path)
{
	setColumnCount(4);
	setRowCount(0);

	setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});

	switch (type)
	{
		case Local:
			initLocal();
			break;
		case Distant:
			initDistant();

	}

}

void RcloneFileModel::initDistant()
{
	auto *drive = new TreeFileItem(path);
	m_root_index = drive->index();
	appendRow({
				  drive,
				  new QStandardItem("--"),
				  new QStandardItem("--"),
				  new QStandardItem(tr("Disque"))
			  });
	addItemDistant(path, drive);


}

QList<QStandardItem *> getItemList(TreeFileItem *item)
{
	return {
		item,
		new TreeFileItem(item->getFile()->getSizeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getModTimeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getIsDirString(), item->getFile(), item)
	};
}

void RcloneFileModel::addItemDistant(const QString &path, TreeFileItem *parent)
{
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItem(path, file.toObject());
			parent->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				addItemDistant(item->getFile()->getPath(), item);
		}
	});
	rclone->lsJson(path.toStdString());
}

void RcloneFileModel::initLocal()
{
	auto *drive = new TreeFileItem(path);
	m_root_index = drive->index();
	drive->appendRow({});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive)
			  });

}

void RcloneFileModel::addItemLocal(const QString &path, TreeFileItem *parent)
{
	if (m_thread not_eq nullptr and m_thread->joinable())
	{
		m_thread->join();
	}
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	m_thread = std::make_shared<std::thread>(
		[=]()
		{
			if (tree_item->rowCount() == 1)
			{
				auto list_file = QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
				for (const QFileInfo &info: list_file)
				{
					auto *item = new TreeFileItem(RcloneFile(info.filePath()));
					tree_item->appendRow(getItemList(item));
					if (info.isDir())
						item->appendRow({});
				}
				if (not list_file.isEmpty())
					tree_item->removeRow(0);
			}
		});
}

const QModelIndex &RcloneFileModel::getRootIndex() const
{
	return m_root_index;
}



