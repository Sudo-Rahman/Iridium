//
// Created by rahman on 05/02/23.
//

#include <QProgressBar>
#include "RcloneFileModelDistant.hpp"
#include "TreeFileItemDistant.hpp"

boost::signals2::signal<void()> RcloneFileModelDistant::_load_change;

RcloneFileModelDistant::RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *View)
        : RcloneFileModel(remoteInfo, View)
{
    RcloneFileModelDistant::init();
    _load_change.connect([this]() {
        for (auto &rclone: _rclones)
            rclone->cancel();
        _rclones.clear();
    });
}

void RcloneFileModelDistant::init()
{
    auto *drive = new TreeFileItem(_remote_info->path.c_str(), _remote_info);
    drive->getFile()->setSize(0);
    drive->setIcon(QIcon(_remote_info->icon.c_str()));
    _root_index = drive->index();
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
    if (loadType() == Iridium::Load::Dynamic)
        addItemDynamic(file->getPath(), parent);
    else
        addItemStatic(file->getPath(), parent);
}

void RcloneFileModelDistant::addItemDynamic(const QString &path, TreeFileItem *parent)
{
    // get rclone instance that is not running
    auto rclone = [this]->RclonePtr{
            for (auto &rclone: _rclones)
            if (not rclone->isRunning())
            return rclone;
            auto rclone = Rclone::create_shared();
            _rclones.push_back(rclone);
            return rclone;
    }();

    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        addProgressBar(tree_item->child(0, 0)->index());
        tree_item->setState(TreeFileItem::Loading);
        connect(rclone.get(), &Rclone::lsJsonFinished, this, [tree_item, path, this](const boost::json::array &array)
        {
            tree_item->removeRow(0);
            for (const auto &file: array)
            {
                auto *item = new TreeFileItemDistant(path, _remote_info, file.as_object());
                tree_item->appendRow(getItemList(item));
            }
            tree_item->setState(TreeFileItem::Loaded);
        });
        connect(rclone.get(), &Rclone::finished, this, [rclone] { rclone->clear(); });
        rclone->lsJson(path.toStdString());
    }
}

void RcloneFileModelDistant::addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth)
{
    if (depth == 0 or loadType() == Iridium::Load::Dynamic)
        return;
    if (depth == maxDepth())
    {
        for (auto &rclone: _rclones) { rclone->cancel(); }
        _rclones.clear();
    }

    auto *tree_item = (parent->getParent() == nullptr ? parent : parent->getParent());
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        auto rclone = RcloneManager::get(true);
        addProgressBar(tree_item->child(0, 0)->index());
        connect(rclone.get(), &Rclone::lsJsonFinished, this,
                [depth, tree_item, rclone, path, this](const boost::json::array &array)
                {
                    tree_item->removeRow(0);
                    for (const auto &file: array)
                    {
                        auto *item = new TreeFileItemDistant(path, _remote_info, file.as_object());
                        tree_item->appendRow(getItemList(item));
                        if (item->getFile()->isDir() and not rclone->isCanceled())
                            addItemStatic(item->getFile()->getPath(), item, depth - 1);
                    }
                    tree_item->setState(TreeFileItem::Loaded);
                });
        connect(rclone.get(), &Rclone::started, this,
                [tree_item] { tree_item->setState(TreeFileItem::Loading); });

        connect(rclone.get(), &Rclone::finished, this, [rclone, this]
        {
            _rclones.erase(std::remove(_rclones.begin(), _rclones.end(), rclone),
                           _rclones.end());
            rclone->disconnect();
        });
        rclone->lsJson(path.toStdString());
        _rclones.push_back(rclone);
        RcloneManager::addLockable(rclone);
    } else
    {
        for (int i = 0; i < tree_item->rowCount(); ++i)
        {
            auto *item = dynamic_cast<TreeFileItemDistant *>(tree_item->child(i, 0));
            if (item == nullptr)
                continue;
            if (item->getFile()->isDir())
                addItemStatic(item->getFile()->getPath(), item, depth - 1);
        }
    }
}
