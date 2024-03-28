//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"
#include <IridiumApp.hpp>
#include <Global.hpp>

boost::signals2::signal<void()> RcloneFileModelDistant::_load_change;

RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr& remoteInfo, QTreeView * View)
	: RcloneFileModel(remoteInfo, View)
{
	RcloneFileModelDistant::init();
	_load_change.connect(
		[this] { stop(); });
}

void RcloneFileModelDistant::init()
{
	auto * drive = new TreeFileItem(RcloneFile(nullptr, "", 0, true, QDateTime::currentDateTime(), _remote_info));
	drive->getFile()->setSize(0);
	drive->setIcon(QIcon(_remote_info->icon().c_str()));
	_root_index = drive->index();
	drive->appendRow(TreeFileItem::decorateList());
	appendRow({
				drive,
				new TreeFileItem(1, drive->getFile()),
				new TreeFileItem(2, drive->getFile()),
				new TreeFileItem(3, drive->getFile()),
		});
}

void RcloneFileModelDistant::addItem(const RcloneFilePtr& file, TreeFileItem * parent)
{
	RcloneFileModel::addItem(file, parent);
	if (loadType() == Iridium::Load::Dynamic)
		addItemDynamic(file, parent);
	else
		addItemStatic(file, parent);
}

void RcloneFileModelDistant::addItemDynamic(const RcloneFilePtr& file, TreeFileItem * parent)
{
	auto * tree_item = parent->siblingAtFirstColumn();
	if (tree_item->state() == TreeFileItem::NotLoaded)
	{
		auto rclone = std::make_unique<ir::process>();
		auto parser = ir::parser::file_parser::create(
			new ir::parser::file_parser(file.get(), [=](const ire::file& f)
			{
				auto * item = new TreeFileItemDistant(RcloneFile(f));
				tree_item->appendRow(getItemList(item));
			}));
		rclone->lsjson(*file).every_line_parser(parser);
		connectProcess(rclone.get(), tree_item);
		_process_pool.add_process(std::move(rclone));
	}
}

void RcloneFileModelDistant::addItemStatic(const RcloneFilePtr& file, TreeFileItem * parent, uint8_t depth)
{
	if (depth == 0 or loadType() == Iridium::Load::Dynamic)
		return;
	if (depth == maxDepth())
		stop();

	auto * tree_item = parent->siblingAtFirstColumn();
	if (tree_item == nullptr)return;
	if (tree_item->state() == TreeFileItem::NotLoaded)
	{
		auto rclone = ir::process_uptr(new ir::process());
		auto parser = ir::parser::file_parser::create(
			new ir::parser::file_parser(file.get(), [=](const ire::file& f)
			{
				auto * item = new TreeFileItemDistant(RcloneFile(f));
				tree_item->appendRow(getItemList(item));
				if (item->getFile()->isDir() and not _stop)
					addItemStatic(item->getFile(), item, depth - 1);
			}));
		rclone->lsjson(*file).every_line_parser(parser);
		connectProcess(rclone.get(), tree_item);
		_process_pool.add_process(std::move(rclone));
	}
	else
	{
		for (int i = 0; i < tree_item->rowCount(); ++i)
		{
			auto * item = dynamic_cast<TreeFileItemDistant *>(tree_item->child(i, 0));
			if (item == nullptr)
				continue;
			if (item->getFile()->isDir())
				addItemStatic(item->getFile(), item, depth - 1);
		}
	}
}


void RcloneFileModelDistant::reload(TreeFileItem * parent)
{
	if (parent->state() == TreeFileItem::Loading)
		return;
	auto * tree_item = parent->siblingAtFirstColumn();
	if (tree_item == nullptr)
		return;
	if (tree_item->state() == TreeFileItem::Loaded)
	{
		tree_item->setState(TreeFileItem::NotLoaded);
		auto rclone = ir::process_uptr(new ir::process());
		auto files = filesInFolder(tree_item);
		// create pointer to files
		auto * files_ptr = new QList<RcloneFilePtr>();
		for (const auto& file: files)
			files_ptr->append(file);

		auto * items = new std::vector<QList<QStandardItem *>>();


		auto parser = ir::parser::file_parser::create(
			new ir::parser::file_parser(tree_item->getFile().get(), [items,tree_item,files_ptr](const ire::file& f)
			{
				auto * item = new TreeFileItemDistant(RcloneFile(f));
				files_ptr->removeIf([item](const RcloneFilePtr& file) { return *(item->getFile()) == *file; });
				if (not fileInFolder(item->getFile(), tree_item))
					items->push_back(getItemList(item));
				else
					delete item;
			}));

		auto on_finish = [items, files_ptr, tree_item, this](auto)
		{
			for (const auto& file: *files_ptr)
				tree_item->removeRow(getTreeFileItem(file, tree_item)->row());
			for (const auto& item: *items)
				tree_item->appendRow(item);
			items->clear();
			delete items;
			delete files_ptr;
			tree_item->setState(TreeFileItem::Loaded);
		};
		rclone->every_line_parser(std::move(parser)).on_finish(std::move(on_finish));
		rclone->lsjson(*tree_item->getFile());
		_process_pool.add_process(std::move(rclone));
	}
}

/**
 * @brief connect rclone signals to tree_item
 * @param process
 * @param tree_item
 */
void RcloneFileModelDistant::connectProcess(ir::process * process, TreeFileItem * tree_item, bool progress)
{
	process->on_start([=, this]
	{
		if (progress)
			addProgressBar(tree_item->child(0, 0)->index());
		tree_item->setState(TreeFileItem::Loading);
	});
	// connect(rclone.get(), &Rclone::killed, this, [tree_item] { tree_item->setState(TreeFileItem::NotLoaded); });
	process->on_finish([=](int exit)
	{
		if (exit == 0 and progress)tree_item->removeRow(0);
		tree_item->setState(TreeFileItem::Loaded);
	});
}
