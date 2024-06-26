//
// Created by Rahman on 30/03/2023.
//

#include "RcloneFileModelLocal.hpp"
#include "TreeFileItemLocal.hpp"
#include <Settings.hpp>
#include <Utility/Utility.hpp>

RcloneFileModelLocal::RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QTreeView *View) : RcloneFileModel(
	remoteInfo, View) { RcloneFileModelLocal::init(); }

void RcloneFileModelLocal::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
	RcloneFileModel::addItem(file, parent);
	if (_thread not_eq nullptr)
	{
		_thread->interrupt();
		_thread->join();
	}
	auto *tree_item = parent->siblingAtFirstColumn();
	if (tree_item->state() == TreeFileItem::NotLoaded)
	{
		tree_item->setState(TreeFileItem::Loading);
		if (_current_item and _current_item not_eq tree_item)
			_current_item->setState(TreeFileItem::NotLoaded);
		_current_item = tree_item;
		_thread = std::make_unique<boost::thread>(
			[this, tree_item, file]()
			{
				if (dynamic_cast<TreeFileItemLocal *>(tree_item->child(0)) == nullptr) tree_item->removeRow(0);

				auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
				auto files = filesInFolder(tree_item);
				for (const QFileInfo &info: list_file)
				{
					boost::this_thread::interruption_point();
					auto *item = new TreeFileItemLocal(RcloneFile(file.get(), info.fileName(),
					                                              info.size(), info.isDir(),
					                                              info.lastModified(), _remote_info));
					files.removeIf([item](const RcloneFilePtr &file) { return *(item->getFile()) == *file; });
					if (not fileInFolder(item->getFile(), tree_item))
						tree_item->appendRow(getItemList(item));
					else
						delete item;
				}
				for (const auto &file: files)
					tree_item->removeRow(getTreeFileItem(file, tree_item)->row());

				tree_item->setState(TreeFileItem::Loaded);
			});
	}
}

void RcloneFileModelLocal::init()
{
	auto *local = new TreeFileItem(RcloneFile(nullptr, "", 0, true, QDateTime::currentDateTime(), _remote_info));
	local->setText(_remote_info->full_path().c_str());
	local->getFile()->setSize(0);
	local->setIcon(Settings::hardDriveIcon());
	_root_index = local->index();
	local->appendRow(TreeFileItem::decorateList());

	appendRow({
				local,
				new TreeFileItem(1, local->getFile()),
				new TreeFileItem(2, local->getFile()),
				new TreeFileItem(3, local->getFile())
		});
}

void RcloneFileModelLocal::reload(TreeFileItem *item)
{
	if (item->state() == TreeFileItem::Loading)
		return;
	item->setState(TreeFileItem::NotLoaded);
	addItem(item->getFile(), item);
}
