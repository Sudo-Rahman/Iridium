//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"

uint8_t RcloneFileModelDistant::m_max_depth = 2;
RcloneFileModelDistant::Load RcloneFileModelDistant::m_load = Dynamic;

RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *View)
        : RcloneFileModel(remoteInfo, View)
{
    RcloneFileModelDistant::init();
}

void RcloneFileModelDistant::init()
{
    auto *drive = new TreeFileItem(m_remote_info->m_path.c_str(), m_remote_info);
    drive->getFile()->setSize(0);
    drive->setIcon(QIcon(m_remote_info->m_icon.c_str()));
    m_root_index = drive->index();
    drive->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
    appendRow({
                      drive,
                      new TreeFileItem(1, drive->getFile(), drive),
                      new TreeFileItem(2, drive->getFile(), drive),
                      new TreeFileItem(3, drive->getFile(), drive),
              });
}

void RcloneFileModelDistant::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    RcloneFileModel::addItem(file, parent);
    if (m_load == Dynamic)
        addItemDynamic(file->getPath(), parent);
    else
        addItemStatic(file->getPath(), parent);
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{
    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    auto rclone = RcloneManager::get();
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        addProgressBar(tree_item->child(0, 0)->index());
        tree_item->setState(TreeFileItem::Loading);
        connect(rclone.get(), &Rclone::lsJsonFinished, this, [tree_item, path, this](const QJsonDocument &doc)
        {
            tree_item->removeRow(0);
            for (const auto &file: doc.array())
            {
                auto *item = new TreeFileItemDistant(path, m_remote_info, file.toObject());
                tree_item->appendRow(getItemList(item));
                if (item->getFile()->isDir())
                    item->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
            }
            tree_item->setState(TreeFileItem::Loaded);
        });
        rclone->lsJson(path.toStdString());
    }
}

void RcloneFileModelDistant::addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth)
{
    if (depth == 0)
        return;
    if (depth == m_max_depth)
        RcloneManager::stop(m_locked_rclone);

    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    auto rclone = RcloneManager::getLockable();
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        addProgressBar(tree_item->child(0, 0)->index());
        connect(rclone.get(), &Rclone::lsJsonFinished, this,
                [depth, tree_item, path, parent, this](const QJsonDocument &doc)
                {
                    tree_item->removeRow(0);
                    for (const auto &file: doc.array())
                    {
                        auto *item = new TreeFileItemDistant(path, m_remote_info, file.toObject());
                        parent->appendRow(getItemList(item));
                        if (item->getFile()->isDir())
                        {
                            item->appendRow(
                                    {new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
                            addItemStatic(item->getFile()->getPath(), item, depth - 1);
                        }
                    }
                    tree_item->setState(TreeFileItem::Loaded);
                });
        connect(rclone.get(), &Rclone::started, this,
                [tree_item] { tree_item->setState(TreeFileItem::Loading); });

        RcloneLocked rcloneLocked = {rclone, [rclone, path] { rclone->lsJson(path.toStdString()); }};

        m_locked_rclone.push_back(rcloneLocked);
        RcloneManager::launch(rcloneLocked);
    }
}
