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
#include <QLineEdit>
#include <QDialogButtonBox>
#include <Settings.hpp>


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
	header()->setFont(QFont("Arial", 13, QFont::DemiBold));
	header()->setSortIndicatorShown(true);
	header()->setSectionsClickable(true);
	header()->setStretchLastSection(false);

	// set row height
	setItemDelegate(new MyItemDelegate(this));

	connectSignals();
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
		if (logicalIndex == 1 and newSize < 100)
			setColumnWidth(1, 100);
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
}

/**
 * @brief resize event
 * @param event
 */
void TreeFileView::resizeEvent(QResizeEvent *event)
{
	QAbstractItemView::resizeEvent(event);
	if (header() == nullptr)
		return;
	if(header()->count() == 0)
		return;
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
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
		QTreeView::setRootIndex(dynamic_cast<RcloneFileModel *>(model)->getRootIndex());
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
	QTreeView::expand(index);
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
}

/**
 * @brief double click on item
 * @param index
 */
void TreeFileView::doubleClick(const QModelIndex &index)
{
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
	dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
	if (!item->getFile()->isDir())
		return;
	auto id = item->getParent() == nullptr ? index.parent() : model->indexFromItem(item->getParent()).parent();
	indexBack.push_back(id);
	indexTop.clear();
	QTreeView::setRootIndex(index);
	emit pathChanged(getPath());
}

/**
 * @brief set model
 * @param model
 */
void TreeFileView::setModel(RcloneFileModel *model)
{
	TreeFileView::model = model;
	QTreeView::setModel(TreeFileView::model);
}

/**
 * @brief mouse release event
 * @param event
 */
void TreeFileView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	if (event->button() == Qt::RightButton)
	{
		ItemMenu menu(this);
		auto lisItem = getSelectedItems();
		if (getSelectedItems(true).isEmpty() and getPath().isEmpty())
			return;
		if (lisItem.size() > 1)
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
				dialog->show();
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
				auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(
					index)));
				lst << item;
//				qDebug() << item->getFile()->getPath();
			}
			deleteFile(lst);
		});
		connect(&menu, &ItemMenu::newFolder, this, [this, lisItem]()
		{
			auto *item = lisItem.first();
			mkdir();
		});
		menu.exec(event->globalPosition().toPoint());
	}
}

/**
 * @brief function for change remote
 * @param info
 */
void TreeFileView::changeRemote(const RemoteInfoPtr &info)
{
	m_remoteInfo = info;
	if (model not_eq nullptr)
		delete model;
	if (!m_remoteInfo->isLocal())
		model = new RcloneFileModelDistant(m_remoteInfo, RcloneFileModelDistant::Dynamic);
	else
		model = new RcloneFileModelLocal(m_remoteInfo);
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
	auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
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
		if (fileIsInFolder(item->getFile(), treePaste))
			continue;

		RcloneFilePtr newFile = std::make_shared<RcloneFile>(
			treePaste->getFile()->getPath() + "/" + item->getFile()->getName(),
			item->getFile()->getSize(),
			item->getFile()->isDir(),
			item->getFile()->isDir() ? QDateTime::currentDateTime() : item->getFile()->getModTime(),
			m_remoteInfo
		);
		auto rclone = m_rclonesManager.get();
		connect(rclone.get(), &Rclone::finished, this, [newFile, treePaste](int exit)
		{
			if (exit == 0)
			{
				auto *treeItem = new TreeFileItem(newFile->getName(), newFile, treePaste, true);
				auto list = RcloneFileModel::getItemList(treeItem);
				if (newFile->isDir())
					treeItem->appendRow({});
				treePaste->appendRow(list);
			}
		});
		rclone->copyTo(item->getFile().operator*(), newFile.operator*());
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
		auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
		lst << item;
	}
	if (can_be_empty)
		return lst;
	if (lst.isEmpty())
	{
		auto index = QTreeView::rootIndex();
		if (index.isValid())
		{
			auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
			lst << item;
		} else
		{
			index = model->index(0, 0, QTreeView::rootIndex());
			auto *item = dynamic_cast<TreeFileItem *>(static_cast<QStandardItem *>(model->itemFromIndex(index)));
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
		auto rclone = m_rclonesManager.get();
		connect(rclone.get(), &Rclone::fileDeleted, this, [this, files, item, rclone](const int exit)
		{
			if (exit == 0)
			{
				m_rclonesManager.release(rclone);
//				QMessageBox::information(this, tr("Suppression"), files.join(", ") + tr(" Supprimé"));
				removeItem(item);
			} else
			{
				QMessageBox::critical(this, tr("Suppression"), files.join(", ") + tr(" non supprimé"));
			}
		});
		rclone->deleteFile(item->getFile().operator*());
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
			dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
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
		dynamic_cast<RcloneFileModel *>(model)->addItem(item->getFile(), item);
	}
}

bool TreeFileView::fileIsInFolder(const RcloneFilePtr &file, TreeFileItem *folder)
{
	if (folder == nullptr)
		exit(1);
	// for each all children
	for (int i = 0; i < folder->rowCount(); i++)
	{
		auto *child = dynamic_cast<TreeFileItem *>(folder->child(i));
		if (child not_eq nullptr)
			if (file->getName() == folder->child(i)->text())
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
		getPath() + "/" + name,
		0,
		true,
		QDateTime::currentDateTime(),
		m_remoteInfo
	);
	if (fileIsInFolder(rcloneFile, items.first()))
	{
		auto msgb = QMessageBox(QMessageBox::Critical, tr("Création"), tr("Le dossier existe déjà"),
								QMessageBox::Ok);
		msgb.exec();
		return;
	}
	auto rclone = m_rclonesManager.get();
	auto *newItem = new TreeFileItem(name, rcloneFile, items.first(), true);
	connect(rclone.get(), &Rclone::finished, this, [this, rclone, name, newItem, items](const int exit)
	{
		if (exit == 0)
		{
			m_rclonesManager.release(rclone);
			// create new item
			auto item_list = RcloneFileModel::getItemList(newItem);
			items.first()->appendRow(item_list);
		} else
		{
			auto msgb = QMessageBox(QMessageBox::Critical, tr("Création"), tr("Le dossier n’a pas pu être créé"),
									QMessageBox::Ok, this);
			msgb.setDetailedText(QString::fromStdString(rclone->readAllError()));
			msgb.exec();
		}
	});
	rclone->mkdir(newItem->getFile().operator*());
}
