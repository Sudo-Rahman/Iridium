//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"

RcloneFileModel::RcloneFileModel(const QString &path, QObject *parent) : QStandardItemModel(
	parent), path(path)
{
	setColumnCount(4);
	setRowCount(0);

	setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});


}

QList<QStandardItem *> RcloneFileModel::getItemList(TreeFileItem *item)
{
	return {
		item,
		new TreeFileItem(item->getFile()->getSizeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getModTimeString(), item->getFile(), item),
		new TreeFileItem(item->getFile()->getIsDirString(), item->getFile(), item)
	};
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


