//
// Created by sr-71 on 07/05/2023.
//

#include "SearchTableView.hpp"
#include <QHeaderView>
#include <Rclone.hpp>
#include <SearchTableItem.hpp>
#include <QMenu>
#include <Variable.hpp>
#include <QDirIterator>


SearchTableView::SearchTableView(QWidget *parent) : QTableView(parent)
{

    m_model = new QStandardItemModel(0, 5, this);

    m_model->setHorizontalHeaderLabels({tr("Chemin"), tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});

    QTableView::setModel(m_model);
    QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
    QTableView::setSelectionMode(QAbstractItemView::ContiguousSelection);
    QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
    QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setMinimumSectionSize(150);
    horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);

    QTableView::setSortingEnabled(true);

    m_model->setSortRole(Qt::UserRole + 1);

    connect(this, &QTableView::customContextMenuRequested, this, &SearchTableView::showCustomContextMenu);
}

void SearchTableView::showCustomContextMenu()
{
    auto items = QTableView::selectedIndexes();
    if (items.empty())
        return;
    auto menu = QMenu(this);
    auto copie = menu.addAction(tr("Copier les fichiers"));
    copie->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    switch (items.size() / horizontalHeader()->count())
    {
        case 1:
        {
            auto parent = menu.addAction(tr("Copier le dossier parent"));
            parent->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
            copie->setText(tr("Copier le fichier"));
        }
            break;
        default:
            break;

    }
    auto action = menu.exec(QCursor::pos());

    if (action == copie)
    {
        std::vector<RcloneFilePtr> files;
        for (auto &item: items)
        {
            auto item_cast = dynamic_cast<SearchTableItem *>(m_model->itemFromIndex(item));
            files.push_back(item_cast->getFile());
        }
        Iridium::Variable::clear_and_swap_copy_files(files);
    }

}

void SearchTableView::addFile(const boost::json::object &file, const RemoteInfoPtr &remoteInfo)
{
    RcloneFilePtr rcloneFile = std::make_shared<RcloneFile>(
            (remoteInfo->m_path + std::string(file.at("Path").as_string())).c_str(),
            file.at("Size").as_int64(),
            file.at("IsDir").as_bool(),
            QDateTime::fromString(file.at("ModTime").as_string().c_str(), Qt::ISODate),
            remoteInfo
    );
    QList<QStandardItem *> row = {
            new SearchTableItem(0, rcloneFile),
            new SearchTableItem(1, rcloneFile),
            new SearchTableItem(2, rcloneFile),
            new SearchTableItem(3, rcloneFile),
            new SearchTableItem(4, rcloneFile)
    };
    m_model->appendRow(row);
}

void SearchTableView::search(const QString &text, const RemoteInfoPtr &remoteInfo)
{
    m_model->removeRows(0, m_model->rowCount());
    if (text.isEmpty())
        return;
    if (not remoteInfo->isLocal())
    {
        m_rclone = RcloneManager::get();
        connect(m_rclone.get(), &Rclone::searchRefresh, this, [this, remoteInfo](const boost::json::object &file)
        {
            addFile(file, remoteInfo);
        });
        connect(m_rclone.get(), &Rclone::finished, this, &SearchTableView::searchFinished);
        m_rclone->search(text.toStdString(), *remoteInfo);
    } else
    {
        if (m_thread.joinable())
        {
            m_thread.interrupt();
            m_thread.join();
        }
        m_thread = boost::thread(
                [this, text, remoteInfo]
                {
                    QDirIterator it(remoteInfo->m_path.c_str(),
                                    QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot,
                                    QDirIterator::Subdirectories);
                    while (it.hasNext())
                    {
                        boost::this_thread::interruption_point();
                        it.next();
                        if (it.fileName().contains(text, Qt::CaseInsensitive))
                        {
                            RcloneFilePtr rcloneFile = std::make_shared<RcloneFile>(
                                    it.filePath(),
                                    it.fileInfo().size(),
                                    it.fileInfo().isDir(),
                                    it.fileInfo().lastModified(),
                                    remoteInfo
                            );
                            m_model->appendRow({
                                                       new SearchTableItem(0, rcloneFile),
                                                       new SearchTableItem(1, rcloneFile),
                                                       new SearchTableItem(2, rcloneFile),
                                                       new SearchTableItem(3, rcloneFile),
                                                       new SearchTableItem(4, rcloneFile)
                                               });
                        }
                    }
                    emit searchFinished();
                });
    }
}
