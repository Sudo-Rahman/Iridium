//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>
#include <QMouseEvent>
#include "TreeFileView.hpp"
#include "RcloneFileModelDistant.hpp"
#include "RcloneFileModelLocal.hpp"
#include "ItemMenu.hpp"
#include "RcloneProxy.hpp"
#include "ItemInfoDialog.hpp"
#include <QEvent>
#include <QItemDelegate>
#include <QPainter>
#include <chrono>
#include <QLineEdit>
#include <QLayout>
#include <QDialogButtonBox>
#include <Settings.hpp>


using namespace std::chrono;

/**
 * @brief Classe permettant de définir la taille des items
 */
class MyItemDelegate : public QItemDelegate
{
public:
	explicit MyItemDelegate(QObject *parent = nullptr) : QItemDelegate(parent)
	{}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{ QItemDelegate::paint(painter, option, index); }

	[[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{ return {30, 30}; }
};


/**
 * @brief Initialise l'interface
 */
void TreeFileView::initUI()
{
	setIndentation(15);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
//	setFocusPolicy(Qt::NoFocus);
	setIconSize(QSize(24, 24));
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
	connect(header(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex, Qt::SortOrder order)
	{
		sortByColumn(logicalIndex, order);
	});

	connect(this, &QTreeView::doubleClicked, this, &TreeFileView::doubleClick);

	connect(this, &QTreeView::expanded, this, &TreeFileView::expand);

	connect(this, &QTreeView::collapsed, [this](const QModelIndex &index)
	{
		// collapse all children
		for (int i = 0; i < model->rowCount(index); i++)
		{
			auto child = model->index(i, 0, index);
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
}

/**
 * @brief Back to previous folder
 */
void TreeFileView::back()
{
	if (indexBack.length() > 0)
	{
		auto index = indexBack.back();
		indexBack.pop_back();
		indexTop << QTreeView::rootIndex();
		QTreeView::setRootIndex(index);
	} else
		QTreeView::setRootIndex(model->getRootIndex());
	emit pathChanged(getPath());
}

/**
 * @brief Go to next folder
 */
void TreeFileView::front()
{
	if (indexTop.length() > 0)
	{
		auto index = indexTop.back();
		indexTop.pop_back();
		indexBack << index.parent();
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
	auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));

	model->addItem(item->getFile(), item);
	QTreeView::expand(index);
}

/**
 * @brief double click on item
 * @param index
 */
void TreeFileView::doubleClick(const QModelIndex &index)
{
	auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));

	if (item == nullptr)
		return;

	model->addItem(item->getFile(), item);
	if (!item->getFile()->isDir())
		return;
	// get parent index
	auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
	indexBack.push_back(id);
	indexTop.clear();
	// if item getParent is null,it's first column item, else we get first column item.
	QTreeView::setRootIndex(item->getParent() == nullptr ? index : model->indexFromItem(item->getParent()));
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
	if (getSelectedItems(true).isEmpty() and getPath().isEmpty())
		return;
	if (lisItem.size() > 1 or not lisItem.first()->getFile()->isDir())
		menu.setActionEnabled({{ItemMenu::Action::Paste, false},
							   {ItemMenu::NewFolder,     false}});
	if (model->indexFromItem(lisItem.first()) == rootIndex())
		menu.setActionEnabled({{ItemMenu::Delete, false}});


	connect(&menu, &ItemMenu::info, this, [lisItem, this]()
	{
		for (auto item: lisItem)
		{
			auto *dialog = new ItemInfoDialog(item, this);
			dialog->move(QPoint(rand() % 1000, rand() % 1000));
			dialog->exec();
		}
	});

	connect(&menu, &ItemMenu::copyed, this, [this, lisItem]()
	{
//			qDebug() << lisItem.first()->getFile()->getName();
		emit fileCopied(lisItem);
	});
	connect(&menu, &ItemMenu::pasted, this, [this, lisItem]()
	{
//			qDebug() << lisItem.first()->getFile()->getPath();
		emit pasted(lisItem.first()->getFile());
	});
	connect(&menu, &ItemMenu::deleted, this, [this, lisItem]()
	{
		QList<TreeFileItem *> lst;
		for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
		{
			auto index = QTreeView::selectedIndexes().at(i);
			auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
			lst << item;
//				qDebug() << item->getFile()->getPath();
		}
		deleteFile(lst);
	});
	connect(&menu, &ItemMenu::newFolder, this, [this, lisItem]()
	{ mkdir(); });
	menu.exec(QCursor::pos());
}

/**
 * @brief function for change remote
 * @param info
 */
void TreeFileView::changeRemote(const RemoteInfoPtr &info)
{
	m_remoteInfo = info;
	delete model;
	if (!m_remoteInfo->isLocal())
		model = new RcloneFileModelDistant(m_remoteInfo, this);
	else
		model = new RcloneFileModelLocal(m_remoteInfo, this);

	connect(model, &RcloneFileModel::itemChanged, this,
			[this](const QStandardItem *item)
			{
				auto index = model->indexFromItem(item);
				if (index == m_clickIndex)
				{
					if (fileIsInFolder(item->text(), getSelectedItems().first()))
						return;

					if (item != m_editingItem)
						rename(dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(
								   index))),
							   item->text());
				}
			});
	QTreeView::setModel(model);
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
	auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
	return item->getFile()->getPath();
}

/**
 * @brief paste items in current folder or in selected folder
 * @param items
 */
void TreeFileView::copyto(const QList<TreeFileItem *> &items)
{
	auto treePaste = getSelectedItems().first();
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
			m_remoteInfo
		);
		auto rclone = RcloneManager::get();
		connect(rclone.get(), &Rclone::finished, this, [newFile, treePaste](int exit)
		{
			if (exit == 0)
			{
				auto *treeItem = new TreeFileItem(newFile->getName(), newFile, nullptr, true);
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
		auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
		lst << item;
	}
	if (can_be_empty)
		return lst;
	if (lst.isEmpty())
	{
		auto index = QTreeView::rootIndex();
		if (index.isValid())
		{
			auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
			lst << item;
		} else
		{
			index = model->index(0, 0, QTreeView::rootIndex());
			auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
			lst << item;
		}
	}
	return lst;
}

/**
 * @brief key press event, shortcut
 * @param event
 */
void TreeFileView::keyPressEvent(QKeyEvent *event)
{
	auto lisItem = getSelectedItems(true);

	if (lisItem.isEmpty() and getPath().isEmpty())
		return;
	if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Copy) and !lisItem.empty())
		emit fileCopied(lisItem);
	if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Paste) and getSelectedItems().size() == 1)
		emit pasted(getSelectedItems().first()->getFile());
	if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_Backspace) and !lisItem.empty())
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
	auto *msgb = new QMessageBox(QMessageBox::Question, tr("Suppression"),
								 tr("Suppression de ") + QString::number(files.size()) + tr(" fichiers"),
								 QMessageBox::Yes | QMessageBox::No, this);

	msgb->setDefaultButton(QMessageBox::No);
	msgb->setDetailedText(files.join("\n"));
	msgb->setInformativeText(tr("Cette action est irréversible"));
	msgb->exec();
	if (msgb->result() == QMessageBox::No)
	{
		msgb->deleteLater();
		return;
	}
	msgb->deleteLater();
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
	model->removeRow(item->row(), model->indexFromItem(item).parent());
}

/**
 * @brief reload current folder or selected folders
 */
void TreeFileView::reload()
{
	auto index = QTreeView::rootIndex();
	if (!index.isValid())
		return;

	// all tree selected
	for (auto item: getSelectedItems())
	{
		if (!item->getFile()->isDir())
			return;

		if (dynamic_cast<TreeFileItem *>(item->child(0)) == nullptr)
		{
			model->addItem(item->getFile(), item);
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
		model->addItem(item->getFile(), item);
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
	icon->setPixmap(Settings::DIR_ICON.pixmap(64));
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
		m_remoteInfo
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
	auto *newItem = new TreeFileItem(name, rcloneFile, items.first(), true);
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
	if (model->index(index.row(), 0, index.parent()) not_eq index)
		return;
	auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
	if (item == nullptr)
		return;
	// if not the first element of treeWidget
	if (!index.parent().isValid())
		return;
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
		} else
			model->itemFromIndex(item->index())->setText(item->getFile()->getName());
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
	m_clickTime = QDateTime::currentMSecsSinceEpoch();
	QTreeView::mousePressEvent(event);
}