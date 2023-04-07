//
// Created by sr-71 on 30/03/2023.
//

#include "RcloneFileModelLocal.hpp"
#include "TreeFileItemLocal.hpp"
#include "../../Config/Settings.hpp"


RcloneFileModelLocal::RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QObject *parent) : RcloneFileModel(
	remoteInfo, parent)
{
	RcloneFileModelLocal::init();
}

void RcloneFileModelLocal::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	if (m_thread not_eq nullptr)
	{
		m_thread->quit();
		m_thread->wait();
		delete m_thread;
	}
	auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
	m_thread = QThread::create(

//	m_thread = std::make_shared<std::thread>(
		[=]()
		{
			if (tree_item->rowCount() == 1)
			{
				auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
				for (const QFileInfo &info: list_file)
				{
					auto *item = new TreeFileItemLocal(info.filePath(), m_remoteInfo);
					tree_item->appendRow(getItemList(item));
					if (info.isDir())
						item->appendRow({});
				}
				tree_item->removeRow(0);
			}
		});
	m_thread->start(QThread::HighestPriority);
}

void RcloneFileModelLocal::init()
{
	auto *drive = new TreeFileItem(QString::fromStdString(m_remoteInfo->m_path), m_remoteInfo);
	drive->setIcon(Settings::HARDDRIVE_ICON);
	m_root_index = drive->index();
	drive->appendRow({});
	appendRow({
				  drive,
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem("--", drive->getFile(), drive),
				  new TreeFileItem(tr("Disque"), drive->getFile(), drive)
			  });
}
