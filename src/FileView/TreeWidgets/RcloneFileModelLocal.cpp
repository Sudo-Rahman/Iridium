//
// Created by Rahman on 30/03/2023.
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
    RcloneFileModel::addItem(file, parent);
    if (_thread not_eq nullptr)
    {
        _thread->interrupt();
        _thread->join();
    }
    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        tree_item->setState(TreeFileItem::Loading);
        _thread = std::make_unique<boost::thread>(
                [this, tree_item, file]()
                {
                    if (dynamic_cast<TreeFileItemLocal *>(tree_item->child(0)) == nullptr) tree_item->removeRow(0);

                    auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
                    auto files = filesInFolder(tree_item);
                    for (const QFileInfo &info: list_file)
                    {
                        boost::this_thread::interruption_point();
                        auto *item = new TreeFileItemLocal(info.filePath(), _remote_info);
                        files.removeIf([item](const RcloneFilePtr &file) { return *(item->getFile()) == *file; });
                        if (not fileInFolder(item->getFile(), tree_item))
                            tree_item->appendRow(getItemList(item));
                        else
                            delete item;
                    }
                    for (const auto &file: files) {
                        tree_item->removeRow(getTreeFileItem(file, tree_item)->row()); }

                    tree_item->setState(TreeFileItem::Loaded);
                });
    }
}

void RcloneFileModelLocal::init()
{
    auto *drive = new TreeFileItem(_remote_info->path.c_str(), _remote_info);
    drive->getFile()->setSize(0);
    drive->setIcon(Settings::hardDriveIcon());
    _root_index = drive->index();
    drive->appendRow(TreeFileItem::decorateList());

    appendRow({
                      drive,
                      new TreeFileItem(1, drive->getFile(), drive),
                      new TreeFileItem(2, drive->getFile(), drive),
                      new TreeFileItem(3, drive->getFile(), drive)
              });
}

void RcloneFileModelLocal::reload(TreeFileItem *item)
{
    if (item->state() == TreeFileItem::Loaded)
    {
        item->setState(TreeFileItem::NotLoaded);
        addItem(item->getFile(), item);
    }
}
