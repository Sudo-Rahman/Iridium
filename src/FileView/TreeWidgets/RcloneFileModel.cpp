//
// Created by Rahman on 02/02/2023.
//

#include "RcloneFileModel.hpp"
#include <QVBoxLayout>
#include <ProgressBar.hpp>
#include <QMessageBox>


/**
 * @brief RcloneFileModel::RcloneFileModel
 * @param remoteInfo
 * @param View
 */
RcloneFileModel::RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View) : m_view(View)
{
    m_remote_info = remoteInfo;

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
            new TreeFileItem(1, item->getFile(), item),
            new TreeFileItem(2, item->getFile(), item),
            new TreeFileItem(3, item->getFile(), item)
    };
}

const QModelIndex &RcloneFileModel::getRootIndex() const
{
    return m_root_index;
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

    if (m_expand_or_double_click)
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
    m_view->setIndexWidget(index, container);
}

RcloneFileModel::RcloneFileModel()
{
    setColumnCount(4);
    setRowCount(0);

    setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
}

void RcloneFileModel::addItem(const RcloneFilePtr &file, TreeFileItem *parent)
{
    if (not m_check_is_valid)
    {
        auto rclone = RcloneManager::get();
        connect(rclone.get(), &Rclone::finished, this, [this, rclone](int exit)
        {
            if (exit not_eq 0)
            {
                auto msgBox = QMessageBox();
                msgBox.setWindowTitle(tr("Erreur"));
                msgBox.setText(
                        tr("Impossible de se connecter au remote %1 !").QString::arg(m_remote_info->name().c_str()));
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setDetailedText(rclone->readAllError().back().c_str());
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
            }
            m_check_is_valid = true;
        });
        rclone->about(m_remote_info.operator*());
    }
}
