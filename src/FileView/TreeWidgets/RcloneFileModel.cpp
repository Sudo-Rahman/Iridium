//
// Created by Rahman on 02/02/2023.
//

#include "RcloneFileModel.hpp"
#include <QVBoxLayout>
#include <ProgressBar.hpp>
#include <QMessageBox>
#include <cmath>

/**
 * @brief RcloneFileModel::RcloneFileModel
 * @param remoteInfo
 * @param View
 */
RcloneFileModel::RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View) : _view(View)
{
    _remote_info = remoteInfo;

    setColumnCount(4);
    setRowCount(0);
    setSortRole(SORT_ROLE);
    setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}

/**
 * @brief return a list of QStandardItem that have the same parent and the same file
 * @param item
 * @return list of QStandardItem
 */
QList<QStandardItem *> RcloneFileModel::getItemList(TreeFileItem *item)
{
    return {
            item,
            new TreeFileItem(1, item->getFile()),
            new TreeFileItem(2, item->getFile()),
            new TreeFileItem(3, item->getFile())
    };
}

const QModelIndex &RcloneFileModel::getRootIndex() const
{
    return _root_index;
}

/**
 * @brief Add a progress bar during the loading of the directory
 * @param index
 */
void RcloneFileModel::addProgressBar(const QModelIndex &index)
{
    auto *container = new QWidget;
    container->setContentsMargins(0, 0, 0, 0);
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    ProgressBar *progressBar;

    if (_expand_or_double_click)
    {
        progressBar = new ProgressBar(ProgressBar::Circular, container);
        progressBar->setFixedSize(40, 40);
    } else
    {
        progressBar = new ProgressBar(ProgressBar::Linear, container);
        progressBar->setFixedSize(100, 15);
    }

    progressBar->setRange(0, 0);
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(progressBar);
    _view->setIndexWidget(index, container);
}

RcloneFileModel::RcloneFileModel()
{
    setColumnCount(4);
    setRowCount(0);

    setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}

void RcloneFileModel::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    if (not _check_is_valid)
    {
        _rclone = Rclone::create_unique();
        connect(_rclone.get(), &Rclone::finished, this, [this](int exit)
        {
            if (exit not_eq 0)
            {
                auto msgBox = QMessageBox(QMessageBox::Critical, tr("Erreur"),
                                          tr("Impossible de se connecter au remote %1 !").arg(
                                                  _remote_info->name().c_str()), QMessageBox::Ok);
                msgBox.setDetailedText(_rclone->readAll().back().c_str());
                msgBox.exec();
            }
            _check_is_valid = true;
        });
        switch (_remote_info->type())
        {
            case iridium::rclone::entity::remote::smb:
                break;
            default:
                _rclone->about(*_remote_info);
                break;
        }
    }
}

QMimeData *RcloneFileModel::mimeData(const QModelIndexList &indexes) const
{
    QList<TreeFileItem *> lst_item;
    int row = -1;
    for (const auto &index: indexes)
    {
        if (index.row() not_eq row)
        {
            row = index.row();
            lst_item << dynamic_cast<TreeFileItem *>(itemFromIndex(index));
        }
    }
//    for (auto *item: lst_item)
//        qDebug() << item->getFile()->getName();

    return new TreeMimeData(lst_item);
}

QStringList RcloneFileModel::mimeTypes() const
{
    return QStandardItemModel::mimeTypes();
}

bool RcloneFileModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                   const QModelIndex &parent)
{
    return QStandardItemModel::dropMimeData(data, action, row, column, parent);
}

bool RcloneFileModel::fileInFolder(const RcloneFilePtr &file, const TreeFileItem *folder)
{
    return std::ranges::any_of(filesInFolder(folder), [file](const RcloneFilePtr &f)
    {
        return *f == *file;
    });
}

bool RcloneFileModel::fileInFolder(const QString &text, const TreeFileItem *folder)
{
    return std::ranges::any_of(filesInFolder(folder), [text](const RcloneFilePtr &f)
    {
        return f->getName() == text;
    });
}


QList<RcloneFilePtr> RcloneFileModel::filesInFolder(const TreeFileItem *folder)
{
    QList<RcloneFilePtr> files;
    for (int i = 0; i < folder->rowCount(); i++)
    {
        auto *item = dynamic_cast<TreeFileItem *>(folder->child(i));
        if (item not_eq nullptr)
            files << item->getFile();
    }
    return files;
}


TreeFileItem *RcloneFileModel::getTreeFileItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    for (int i = 0; i < parent->rowCount(); i++)
    {
        auto *item = dynamic_cast<TreeFileItem *>(parent->child(i));
        if (item == nullptr)
            continue;
        if (*item->getFile() == *file)
            return item;
    }
    return nullptr;
}
