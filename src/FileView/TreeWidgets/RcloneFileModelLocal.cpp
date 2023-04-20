//
// Created by sr-71 on 30/03/2023.
//

#include "RcloneFileModelLocal.hpp"
#include "TreeFileItemLocal.hpp"
#include <Settings.hpp>
#include <Utility/Utility.hpp>


RcloneFileModelLocal::RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QTreeView *View) : RcloneFileModel(
	remoteInfo, View)
{
	RcloneFileModelLocal::init();
}

void RcloneFileModelLocal::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	if (m_thread not_eq nullptr)
	{
		m_thread->interrupt();
		m_thread->join();
	}
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	if (tree_item->rowCount() == 1 and tree_item->state() == TreeFileItem::NotLoaded)
	{
		tree_item->setState(TreeFileItem::Loading);
		m_thread = boost::make_shared<boost::thread>(
			[this, tree_item, file]()
			{
				auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
				for (const QFileInfo &info: list_file)
				{
					boost::this_thread::interruption_point();
					auto *item = new TreeFileItemLocal(info.filePath(), m_remoteInfo);
					tree_item->appendRow(getItemList(item));
					if (info.isDir())
						item->appendRow(
							{new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
				}
				tree_item->removeRow(0);
				tree_item->setState(TreeFileItem::Loaded);
			});
	}
}

void RcloneFileModelLocal::init()
{
	auto *drive = new TreeFileItem(QString::fromStdString(m_remoteInfo->m_path), m_remoteInfo);
	drive->setIcon(Settings::hardDriveIcon());
	m_root_index = drive->index();
	drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive)
			  });
}
