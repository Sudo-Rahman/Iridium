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
    if (m_thread not_eq nullptr)
    {
        m_thread->interrupt();
        m_thread->join();
    }
    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        tree_item->setState(TreeFileItem::Loading);
        m_thread = std::make_unique<boost::thread>(
                [this, tree_item, file]()
                {
                    tree_item->removeRow(0);
                    auto list_file = QDir(file->getPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
                    for (const QFileInfo &info: list_file)
                    {
                        boost::this_thread::interruption_point();
                        auto *item = new TreeFileItemLocal(info.filePath(), m_remote_info);
                        tree_item->appendRow(getItemList(item));
                    }
                    tree_item->setState(TreeFileItem::Loaded);
                });
    }
}

void RcloneFileModelLocal::init()
{
    auto *drive = new TreeFileItem(QString::fromStdString(m_remote_info->m_path), m_remote_info);
    drive->getFile()->setSize(0);
    drive->setIcon(Settings::hardDriveIcon());
    m_root_index = drive->index();
    drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
    appendRow({
                      drive,
                      new TreeFileItem(1, drive->getFile(), drive),
                      new TreeFileItem(2, drive->getFile(), drive),
                      new TreeFileItem(3, drive->getFile(), drive)
              });
}
