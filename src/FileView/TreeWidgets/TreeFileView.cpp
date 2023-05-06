//
// Created by Rahman on 18/01/2023.
//
#include <QHeaderView>
#include "TreeFileView.hpp"
#include "RcloneFileModelDistant.hpp"
#include "RcloneFileModelLocal.hpp"
#include "ItemMenu.hpp"
#include "ItemInfoDialog.hpp"
#include <QEvent>
#include <QItemDelegate>
#include <QPainter>
#include <chrono>
#include <QLineEdit>
#include <QLayout>
#include <QDialogButtonBox>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QSysInfo>
#include <Settings.hpp>


using namespace std::chrono;

/**
 * @brief Classe permettant de définir la taille des items
 */
class MyItemDelegate : public QItemDelegate
{
public:
    explicit MyItemDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override { QItemDelegate::paint(painter, option, index); }

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        return {35, 35};
    }
};


/**
 * @brief Initialise l'interface
 */
void TreeFileView::initUI()
{
    setIndentation(15);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSortingEnabled(true);
    setRootIsDecorated(true);
    setAnimated(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setIconSize(QSize(30, 30));
    setFont(QFont("Arial", 15));
    setAlternatingRowColors(true);
    setUniformRowHeights(true);

    header()->setSectionsMovable(true);
    header()->setFont(QFont("Arial", 13));
    header()->setSortIndicatorShown(true);
    header()->setSectionsClickable(true);
    header()->setStretchLastSection(false);
    setContextMenuPolicy(Qt::CustomContextMenu);

    // set row height
    setItemDelegate(new MyItemDelegate(this));

    connectSignals();

    setColumnWidth(0, 200);
    setColumnWidth(1, 50);

    setDragDropMode(QAbstractItemView::DragDrop);

    auto p = QTreeView::palette();
    p.setColor(QPalette::Highlight, QWidget::palette().color(QPalette::Midlight));
    p.setColor(QPalette::HighlightedText, QWidget::palette().color(QPalette::Text));
    // if other than macos change alternate base color
    if (QSysInfo::productType() not_eq "macos")
        p.setColor(QPalette::AlternateBase, QWidget::palette().color(QPalette::Mid));
    setPalette(p);
}

TreeFileView::TreeFileView(QWidget *parent) : QTreeView(parent)
{
    initUI();
}

/**
 * @brief Constructeur
 * @param remoteInfo
 * @param parent
 */
TreeFileView::TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent) : QTreeView(parent)
{
    initUI();
    changeRemote(remoteInfo);
}

/**
 * @brief Connecte les signaux
 */
void TreeFileView::connectSignals()
{

    connect(this, &QTreeView::doubleClicked, this, &TreeFileView::doubleClick);

    connect(this, &QTreeView::expanded, this, &TreeFileView::expand);

    connect(this, &QTreeView::collapsed, [this](const QModelIndex &index)
    {
        // collapse all children
        for (int i = 0; i < m_model->rowCount(index); i++)
        {
            auto child = m_model->index(i, 0, index);
            collapse(child);
        }
    });

    connect(header(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize)
    {
        if (logicalIndex == 0 and newSize < 200)
            setColumnWidth(0, 200);
        if (logicalIndex == 1 and newSize < 50)
            setColumnWidth(1, 50);
        if (logicalIndex == 2 and newSize < 100)
            setColumnWidth(2, 100);
        if (logicalIndex == 3 and newSize < 100)
        {
            setColumnWidth(3, 100);
            return;
        }
        // get size of all columns
        int size = 0;
        for (int i = 0; i < header()->count(); i++)
            size += header()->sectionSize(i);
        if (size < header()->size().width())
            setColumnWidth(3, header()->size().width() - size + header()->sectionSize(3));
    });

    connect(this, &QTreeView::customContextMenuRequested, this, &TreeFileView::showContextMenu);
}

/**
 * @brief resize event
 * @param event
 */
void TreeFileView::resizeEvent(QResizeEvent *event)
{
    if (header()->count() > 0)
        header()->setSectionResizeMode(0, QHeaderView::Stretch);
    QAbstractItemView::resizeEvent(event);
    if (header()->count() > 0)
        header()->setSectionResizeMode(0, QHeaderView::Interactive);
    if (header()->sectionSize(0) < 200)
        setColumnWidth(0, 200);
}

/**
 * @brief Back to previous folder
 */
void TreeFileView::back()
{
    if (m_index_back.length() > 0)
    {
        auto index = m_index_back.back();
        m_index_back.pop_back();
        m_index_front << QTreeView::rootIndex();
        QTreeView::setRootIndex(index);
    } else
        QTreeView::setRootIndex(m_model->getRootIndex());
    emit pathChanged(getPath());
}

/**
 * @brief Go to next folder
 */
void TreeFileView::front()
{
    if (m_index_front.length() > 0)
    {
        auto index = m_index_front.back();
        m_index_front.pop_back();
        m_index_back << index.parent();
        QTreeView::setRootIndex(index);
    }
    emit pathChanged(getPath());
}

/**
 * @brief expand folder
 * @param index
 */
void TreeFileView::expand(const QModelIndex &index)
{
    auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
    m_model->setExpandOrDoubleClick(false);

    m_model->addItem(item->getFile(), item);
    QTreeView::expand(index);
}

/**
 * @brief double click on item
 * @param index
 */
void TreeFileView::doubleClick(const QModelIndex &index)
{
    auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));

    if (item == nullptr)
        return;

    if (not item->getFile()->isDir())
        return;

    search("");

    m_model->setExpandOrDoubleClick(true);
    m_model->addItem(item->getFile(), item);

//     get parent index
    auto id = item->getParent() == nullptr ? index.parent() : m_model->indexFromItem(item->getParent()).parent();
    m_index_back.push_back(id);
    m_index_front.clear();
    // if item getParent is null,it's first column item, else we get first column item.
    QTreeView::setRootIndex(item->getParent() == nullptr ? index : m_model->indexFromItem(item->getParent()));
    emit pathChanged(getPath());
}

/**
 * @brief context menu
 * @param pos
 */
void TreeFileView::showContextMenu()
{
    ItemMenu menu(this);
    auto lisItem = getSelectedItems();
    if (getSelectedItems(true).isEmpty())
        return;
    if (lisItem.size() > 1 or not lisItem.first()->getFile()->isDir())
        menu.setActionEnabled({{ItemMenu::Action::Paste, false},
                               {ItemMenu::NewFolder,     false}});
    if (not QTreeView::currentIndex().parent().isValid())
        menu.setActionEnabled({{ItemMenu::Delete, false}});

    ItemMenu::Action action = menu.exec(QCursor::pos());

    switch (action)
    {
        case ItemMenu::Action::Info:
            for (auto item: lisItem)
            {
                auto *dialog = new ItemInfoDialog(item, this);
                dialog->move(QPoint(rand() % 1000, rand() % 1000));
                dialog->exec();
            }
            break;
        case ItemMenu::Action::Copy:
            emit fileCopied(lisItem);
            break;
        case ItemMenu::Action::Paste:
            emit pasted(lisItem.first()->getFile());
            break;
        case ItemMenu::Action::Delete:
        {
            QList<TreeFileItem *> lst;
            for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
            {
                auto index = QTreeView::selectedIndexes().at(i);
                auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
                lst << item;
            }
            deleteFile(lst);
            break;
        }
        case ItemMenu::Action::NewFolder:
            mkdir();
            break;
        default:
            break;
    }
}

/**
 * @brief function for change remote
 * @param info
 */
void TreeFileView::changeRemote(const RemoteInfoPtr &info)
{
    if (m_remote_info == info)
        return;

    if (info == nullptr)
    {
        QTreeView::setModel(new QStandardItemModel(this));
        return;
    }

    m_remote_info = info;
    delete m_model;
    if (!m_remote_info->isLocal())
        m_model = new RcloneFileModelDistant(m_remote_info, this);
    else
        m_model = new RcloneFileModelLocal(m_remote_info, this);

    connect(m_model, &RcloneFileModel::itemChanged, this,
            [this](const QStandardItem *item)
            {
                auto index = item->index();
                if (item != m_editingItem and index == m_clickIndex and not m_editable)
                {
                    auto *it = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
                    m_model->setData(index, it->getFile()->getName(), Qt::EditRole);
                }
                if (item != m_editingItem and index == m_clickIndex and m_editable)
                    rename(dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(m_model->itemFromIndex(
                            index))), item->text());
            });
    QTreeView::setModel(m_model);
}

/**
 * @brief return path of current folder
 * @return
 */
QString TreeFileView::getPath()
{
    auto index = QTreeView::rootIndex();
    if (!index.isValid())
        return "";
    auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
    return item->getFile()->getPath();
}

/**
 * @brief paste items in current folder or in selected folder
 * @param items
 */
void TreeFileView::copyto(const QList<TreeFileItem *> &items, TreeFileItem *item)
{
    auto treePaste = item == nullptr ? getSelectedItems().first() : item;
    if (!treePaste->getFile()->isDir())
        return;
    for (const auto item: items)
    {
        if (item == nullptr)
            return;
        if (fileIsInFolder(item->getFile()->getName(), treePaste))
            continue;

        RcloneFilePtr newFile = std::make_shared<RcloneFile>(
                treePaste->getFile()->getPath() + item->getFile()->getName(),
                item->getFile()->getSize(),
                item->getFile()->isDir(),
                item->getFile()->isDir() ? QDateTime::currentDateTime() : item->getFile()->getModTime(),
                m_remote_info
        );
        auto rclone = RcloneManager::get();
        connect(rclone.get(), &Rclone::finished, this, [this, newFile, treePaste](int exit)
        {
            if (exit == 0)
            {
                if (treePaste->state() == TreeFileItem::NotLoaded)
                    return;
                if (fileIsInFolder(newFile->getName(), treePaste))
                {
                    reload(treePaste);
                    return;
                }
                auto *treeItem = new TreeFileItem(0, newFile, nullptr);
                auto list = RcloneFileModel::getItemList(treeItem);
                if (newFile->isDir())
                    treeItem->appendRow({});
                treePaste->appendRow(list);
            }
        });
        emit taskAdded(item->getFile()->getPath(), newFile->getPath(), rclone,
                       [rclone, newFile, item]
                       {
                           rclone->copyTo(item->getFile().operator*(), newFile.operator*());
                       }, Rclone::Copy);
    }
}


/**
 * @brief get selected items
 * @param can_be_empty
 * @return selected items or root item if can_be_empty is false
 */
QList<TreeFileItem *> TreeFileView::getSelectedItems(bool can_be_empty)
{
    QList<TreeFileItem *> lst;
    for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
    {
        auto index = QTreeView::selectedIndexes().at(i);
        auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
        if (index not_eq QTreeView::rootIndex() and item not_eq nullptr)
            lst << item;
    }
    if (can_be_empty)
        return lst;
    if (lst.isEmpty())
    {
        auto index = QTreeView::rootIndex();
        TreeFileItem *item;
        if (index.isValid())
            item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
        else
        {
            index = m_model->index(0, 0, QTreeView::rootIndex());
            item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
        }
        if (item not_eq nullptr)
            lst << item;
    }
    return lst;
}

/**
 * @brief key press event, shortcut
 * @param event
 */
void TreeFileView::keyPressEvent(QKeyEvent *event)
{
    // if ctrl + f
    if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_F))
    {
        emit ctrlFPressed();
        return;
    }

    // if ctrl + r for reload
    if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_R))
    {
        reload();
        return;
    }

    auto lisItem = getSelectedItems(true);

    if (lisItem.isEmpty() and getPath().isEmpty())
        return;
    if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Copy) and !lisItem.empty())
        emit fileCopied(lisItem);
    if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Paste) and getSelectedItems().size() == 1)
        emit pasted(getSelectedItems().first()->getFile());
    if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_Backspace) and !lisItem.empty())
        if (QTreeView::currentIndex().parent().isValid())
            deleteFile(lisItem);
    if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_N))
        mkdir();

    switch (event->key())
    {
        case Qt::Key_Space:
        {
            for (auto item: getSelectedItems())
            {
                auto *info = new ItemInfoDialog(item, this);
                // show in random position
                info->move(QPoint(rand() % 1000, rand() % 1000));
                info->show();
            }
        }
            break;
        case Qt::Key_Left:
            back();
            break;
        case Qt::Key_Right:
            front();
            break;
        case Qt::Key_F2:
            if (m_clickIndex not_eq m_model->index(0, 0))
                edit(m_clickIndex);
            break;
    }
}

/**
 * @brief delete file, delete item in model
 * @param items
 */
void TreeFileView::deleteFile(const QList<TreeFileItem *> &items)
{
    auto files = [items]() -> auto
    {
        QStringList lst;
        for (auto item: items)
            lst << item->getFile()->getName();
        return lst;
    }();
    if (files.isEmpty())
        return;
    auto msgb = QMessageBox(QMessageBox::Question, tr("Suppression"),
                            tr("Suppression de ") + QString::number(files.size()) + tr(" fichiers"),
                            QMessageBox::Yes | QMessageBox::No, this);

    msgb.setDefaultButton(QMessageBox::No);
    msgb.setDetailedText(files.join("\n"));
    msgb.setInformativeText(tr("Cette action est irréversible."));
    if (msgb.exec() == QMessageBox::No)
        return;
    for (auto item: items)
    {
        auto rclone = RcloneManager::get();
        connect(rclone.get(), &Rclone::taskFinished, this, [this, files, item, rclone](const int exit)
        {
            if (exit == 0)
            {
                RcloneManager::release(rclone);
                removeItem(item);
            }
        });
        emit taskAdded(item->getFile()->getPath(), "--", rclone, [rclone, item]()
        {
            rclone->deleteFile(item->getFile().operator*());
        }, Rclone::Delete);
    }
}

/**
 * @brief remove item from model
 * @param item
 */
void TreeFileView::removeItem(TreeFileItem *item)
{
    if (item == nullptr)
        return;
    // remove recursively
    if (item->getFile()->isDir())
    {
        for (int i = 0; i < item->rowCount(); i++)
        {
            auto *child = dynamic_cast<TreeFileItem *>(item->child(i));
            if (child not_eq nullptr)
                removeItem(child);
        }
    }
    m_model->removeRow(item->row(), m_model->indexFromItem(item).parent());
}

/**
 * @brief reload current folder or selected folders
 */
void TreeFileView::reload(TreeFileItem *treeItem)
{
    auto lstItem = treeItem == nullptr ? getSelectedItems() : QList<TreeFileItem *>{treeItem};

    // all tree selected
    for (auto item: lstItem)
    {
        if (not item->getFile()->isDir())
            return;

        if (dynamic_cast<TreeFileItem *>(item->child(0)) == nullptr)
        {
            m_model->addItem(item->getFile(), item);
            continue;
        }

        // remove all children
        QList<TreeFileItem *> lst;
        for (int i = 0; i < item->rowCount(); i++)
        {
            auto child = dynamic_cast<TreeFileItem *>(item->child(i));
            if (child not_eq nullptr)
                lst << child;
        }
        for (auto child: lst)
            removeItem(child);
        // add new children
        item->appendRow({new QStandardItem, new QStandardItem, new QStandardItem, new QStandardItem});
        item->setState(TreeFileItem::NotLoaded);
        m_model->addItem(item->getFile(), item);
    }
}

/**
 * @brief check if file is in folder
 * @param file
 * @param folder
 * @return true if file is in folder
 */
bool TreeFileView::fileIsInFolder(const QString &name, TreeFileItem *folder)
{
    if (folder == nullptr)
        exit(1);
    // for each all children
    for (int i = 0; i < folder->rowCount(); i++)
    {
        if (name == folder->child(i)->text())
            return true;
    }
    return false;
}

QDialog *TreeFileView::mkdirDialog()
{
    auto *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Création de dossier"));
    auto *layout = new QGridLayout(dialog);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    auto *icon = new QLabel(dialog);
    icon->setPixmap(Settings::dirIcon().pixmap(64));
    icon->setAlignment(Qt::AlignCenter);
    layout->addWidget(icon, 0, 0, 3, 1);
    auto *name = new QLineEdit(dialog);
    name->setObjectName("name");
    name->setPlaceholderText(tr("Nom du dossier"));
    layout->addWidget(name, 0, 1, 1, 2);
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    layout->addWidget(buttonBox, 2, 1, 1, 2);

    return dialog;
}


/**
 * @brief create a new folder
 */
void TreeFileView::mkdir()
{
    auto *dialog = mkdirDialog();
    dialog->deleteLater();
    if (dialog->exec() not_eq QDialog::Accepted)
        return;
    auto name = dialog->findChild<QLineEdit *>("name")->text();
    if (name.isEmpty())
        return;
    auto items = getSelectedItems();
    auto rcloneFile = std::make_shared<RcloneFile>(
            getPath() + name,
            0,
            true,
            QDateTime::currentDateTime(),
            m_remote_info
    );
//	qDebug() << "mkdir" << rcloneFile->getPath();
    if (fileIsInFolder(rcloneFile->getName(), items.first()))
    {
        auto msgb = QMessageBox(QMessageBox::Critical, tr("Création"), tr("Le dossier existe déjà"),
                                QMessageBox::Ok);
        msgb.exec();
        return;
    }
    auto rclone = RcloneManager::get();
    auto *newItem = new TreeFileItem(0, rcloneFile, items.first());
    connect(rclone.get(), &Rclone::finished, this, [this, rclone, name, newItem, items](const int exit)
    {
        if (exit == 0)
        {
            RcloneManager::release(rclone);
            // create new item
            auto item_list = RcloneFileModel::getItemList(newItem);
            items.first()->appendRow(item_list);
        } else
        {
            auto msgb = QMessageBox(QMessageBox::Critical, tr("Création"), tr("Le dossier n’a pas pu être créé"),
                                    QMessageBox::Ok, this);
            msgb.setDetailedText(QString::fromStdString(rclone->readAllError().back()));
            msgb.exec();
        }
    });
    emit taskAdded("--", newItem->getFile()->getPath(), rclone, [rclone, newItem]
    {
        rclone->mkdir(newItem->getFile().operator*());
    }, Rclone::Mkdir);
}


/**
 * @brief edit item
 * @param index
 */
void TreeFileView::editItem(const QModelIndex &index)
{
    // if not the first column
    if (m_model->index(index.row(), 0, index.parent()) not_eq index)
        return;
    auto *item = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index));
    if (item == nullptr)
        return;
    // if not the first element of treeWidget
    if (!index.parent().isValid())
        return;
    connect(m_model, &RcloneFileModel::beforeDataChanged, this, [this, item, index](const QVariant &data, int role)
    {
        if (role not_eq Qt::EditRole)
            return;
        if (data.toString().isEmpty() or data.toString() == item->getFile()->getName())
            return;
        if (fileIsInFolder(data.toString(), dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(index.parent()))))
        {
            auto msgb = QMessageBox(QMessageBox::Critical, tr("Renommage"), tr("Le fichier existe déjà"),
                                    QMessageBox::Ok, this);
            msgb.exec();
            m_editable = false;
            disconnect(m_model, &RcloneFileModel::beforeDataChanged, this, nullptr);
            return;
        }
        m_editable = true;
    });
    edit(index);
}

/**
 * @brief reanme item
 * @param item
 * @param newName
 */
void TreeFileView::rename(const TreeFileItem *item, const QString &newName)
{
    auto rclone = RcloneManager::get();
    connect(rclone.get(), &Rclone::finished, this, [this, rclone, item, newName](const int exit)
    {
        if (item->index() == QModelIndex())
            return;
        if (exit == 0)
        {
            RcloneManager::release(rclone);
            item->getFile()->changeName(newName);
            reload(const_cast<TreeFileItem *>(item));
        } else
            m_model->itemFromIndex(item->index())->setText(item->getFile()->getName());
        m_editingItem = nullptr;
    });
    m_editingItem = const_cast<TreeFileItem *>(item);
    auto oldFile = *(item->getFile());
    auto newFile = oldFile;
    newFile.changeName(newName);
    emit taskAdded(oldFile.getPath(), newFile.getPath(), rclone, [oldFile, newFile, rclone]()
    {
        rclone->moveto(oldFile, newFile);
    }, Rclone::Rename);
}

void TreeFileView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (getSelectedItems(true).size() == 1)
            selectionModel()->clearSelection();
        if (QDateTime::currentMSecsSinceEpoch() - m_clickTime < 700 and
            QDateTime::currentMSecsSinceEpoch() - m_clickTime > 350)
        {
            if (m_clickIndex == indexAt(event->pos()))
            {
                auto index = indexAt(event->pos());
                if (index.isValid())
                    editItem(index);
            }
        }
    }
    m_clickIndex = indexAt(event->pos());

    m_dragItems = getSelectedItems(true).empty() ?
                  QList{dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(m_clickIndex))}
                                                 : getSelectedItems(true);
    m_clickTime = QDateTime::currentMSecsSinceEpoch();
    QTreeView::mousePressEvent(event);
}

/**
 * @brief drop event
 * @param event
 */
void TreeFileView::dropEvent(QDropEvent *event)
{
    if (not dynamic_cast<TreeFileView *>(event->source())->m_dragable)
    {
        event->ignore();
        return;
    }
    // get items to drop
    auto lst = dynamic_cast<TreeFileView *>(event->source())->getDragItems();
    if (lst.isEmpty())
    {
        event->ignore();
        return;
    }

    // get item destination
    auto item_to_drop = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(indexAt(event->pos()))) == nullptr
                        ? dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(rootIndex()))
                        : dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(indexAt(event->pos())));

    item_to_drop = item_to_drop->getParent() == nullptr ? item_to_drop : item_to_drop->getParent();
    copyto(lst, item_to_drop);
    event->acceptProposedAction();
}

/**
 * @brief drag enter event
 * @param event
 */
void TreeFileView::dragMoveEvent(QDragMoveEvent *event)
{
    auto tree = dynamic_cast<TreeFileView *>(event->source());
    auto index = indexAt(event->pos());
    // select all row
    if (index.isValid())
        selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    else
        selectionModel()->clearSelection();

    auto item_to_drop = dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(indexAt(event->pos()))) == nullptr
                        ? dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(rootIndex()))
                        : dynamic_cast<TreeFileItem *>(m_model->itemFromIndex(indexAt(event->pos())));
    if (item_to_drop == nullptr)
    {
        event->ignore();
        tree->m_dragable = false;
        return;
    }
    if (not item_to_drop->getFile()->isDir())
    {
        event->ignore();
        tree->m_dragable = false;
        return;
    }

    if (item_to_drop->index() == dynamic_cast<TreeFileView *>(event->source())->rootIndex())
    {
        event->ignore();
        tree->m_dragable = false;
        return;
    }

    auto lst = dynamic_cast<TreeFileView *>(event->source())->getDragItems();

    // not drop in itself
    if (not lst.isEmpty())
    {
        for (auto item: lst)
        {
            if (item->index() == item_to_drop->index() or
                fileIsInFolder(item->getFile()->getName(), item_to_drop))
            {
                event->ignore();
                tree->m_dragable = false;
                return;
            }
        }
    }

    tree->m_dragable = true;
    event->accept();
}

/**
 * @brief search item in root index, hide all item not match
 * @param text
 */
void TreeFileView::search(const QString &text)
{
    if (not QTreeView::rootIndex().isValid())
        return;
    for (int i = 0; i < m_model->itemFromIndex(QTreeView::rootIndex())->rowCount(); i++)
    {
        auto item = m_model->itemFromIndex(QTreeView::rootIndex())->child(i);
        if (item->text().contains(text, Qt::CaseInsensitive))
            setRowHidden(i, QTreeView::rootIndex(), false);
        else
            setRowHidden(i, QTreeView::rootIndex(), true);
    }
}