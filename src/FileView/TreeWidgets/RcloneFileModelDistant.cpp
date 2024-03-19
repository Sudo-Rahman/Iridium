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
    _load_change.connect(
            [this]
            {
                stop();
            });
}

void RcloneFileModelDistant::init()
{
    auto *drive = new TreeFileItem(_remote_info->full_path().c_str(), _remote_info);
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

void RcloneFileModelDistant::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    RcloneFileModel::addItem(file, parent);
    if (loadType() == Iridium::Load::Dynamic)
        addItemDynamic(file, parent);
    else
        addItemStatic(file, parent);
}

void RcloneFileModelDistant::addItemDynamic(const RcloneFilePtr &file, TreeFileItem *parent)
{
    // get rclone instance that is not running
    auto rclone = getRclone();
    auto *tree_item = parent->siblingAtFirstColumn();
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        connect(rclone.get(), &Rclone::readDataJson, this, [tree_item, file, this](const boost::json::object &obj)
        {
            auto *item = new TreeFileItemDistant(file->getPath(), _remote_info, obj);
            tree_item->appendRow(getItemList(item));
        });
        connectRclone(rclone, tree_item);
        rclone->lsJson(*file);
    }
}

void RcloneFileModelDistant::addItemStatic(const RcloneFilePtr &file, TreeFileItem *parent, uint8_t depth)
{
    if (depth == 0 or loadType() == Iridium::Load::Dynamic)
        return;
    if (depth == maxDepth())
        stop();

    auto *tree_item = parent->siblingAtFirstColumn();
    if (tree_item == nullptr)return;
    if (tree_item->state() == TreeFileItem::NotLoaded)
    {
        auto rclone = getRclone(false);
        connect(rclone.get(), &Rclone::readDataJson, this,
                [depth, tree_item, rclone, file, this](const boost::json::object &obj)
                {
                    auto *item = new TreeFileItemDistant(file->getPath(), _remote_info, obj);
                    tree_item->appendRow(getItemList(item));
                    if (item->getFile()->isDir() and not _stop)
                        addItemStatic(item->getFile(), item, depth - 1);
                });
        connectRclone(rclone, tree_item);
        rclone->lsJson(*file);
        RcloneManager::addLockable(rclone);
    } else
    {
        for (int i = 0; i < tree_item->rowCount(); ++i)
        {
            auto *item = dynamic_cast<TreeFileItemDistant *>(tree_item->child(i, 0));
            if (item == nullptr)
                continue;
            if (item->getFile()->isDir())
                addItemStatic(item->getFile(), item, depth - 1);
        }
    }
}


void RcloneFileModelDistant::reload(TreeFileItem *parent)
{
    if (parent->state() == TreeFileItem::Loading)
        return;
    auto *tree_item = parent->siblingAtFirstColumn();
    if (tree_item == nullptr)
        return;
    if (tree_item->state() == TreeFileItem::Loaded)
    {
        tree_item->setState(TreeFileItem::NotLoaded);
        auto rclone = getRclone();
        auto files = filesInFolder(tree_item);
        // create pointer to files
        auto *files_ptr = new QList<RcloneFilePtr>();
        for (const auto &file: files)
            files_ptr->append(file);

        auto *items = new std::vector<QList<QStandardItem *>>();

        connect(rclone.get(), &Rclone::readDataJson, this,
                [files_ptr, items, tree_item, this](const boost::json::object &obj)
                {
                    auto *item = new TreeFileItemDistant(tree_item->getFile()->getPath(), _remote_info, obj);
                    files_ptr->removeIf([item](const RcloneFilePtr &file) { return *(item->getFile()) == *file; });
                    if (not fileInFolder(item->getFile(), tree_item))
                        items->push_back(getItemList(item));
                    else
                        delete item;
                });

        connectRclone(rclone, tree_item, false);
        connect(rclone.get(), &Rclone::finished, this, [items, files_ptr, tree_item, this](int exit)
        {
            for (const auto &file: *files_ptr)
                tree_item->removeRow(getTreeFileItem(file, tree_item)->row());
            for (const auto &item: *items)
                tree_item->appendRow(item);
            items->clear();
            delete items;
            delete files_ptr;
        });
        rclone->lsJson(*tree_item->getFile());
    }
}

/**
 * @brief connect rclone signals to tree_item
 * @param rclone
 * @param tree_item
 */
void RcloneFileModelDistant::connectRclone(const RclonePtr &rclone, TreeFileItem *tree_item, bool progress)
{
    connect(rclone.get(), &Rclone::started, this, [progress, tree_item, this]
    {
        if (progress)
            addProgressBar(tree_item->child(0, 0)->index());
        tree_item->setState(TreeFileItem::Loading);
    });
    connect(rclone.get(), &Rclone::killed, this, [tree_item] { tree_item->setState(TreeFileItem::NotLoaded); });
    connect(rclone.get(), &Rclone::finished, this,
            [progress, tree_item](int exit)
            {
                if (exit == 0 and progress)tree_item->removeRow(0);
                tree_item->setState(TreeFileItem::Loaded);
            });
}

/**
 * @brief get rclone instance for dynamic or static load
 * @param dynamic
 * @return rclone instance
 */
RclonePtr RcloneFileModelDistant::getRclone(bool dynamic)
{
    return [this, &dynamic]
    {
        auto rclone = Rclone::create_shared();
        if (dynamic)
        {
            for (auto &r: _rclones_dynamic)
                if (not r->isRunning())
                    return r;
            _rclones_dynamic.push_back(rclone);
            return rclone;
        } else
        {
            _rclones_static.push_back(rclone);
            rclone->setLockable(true);
        }
        return rclone;
    }();
}