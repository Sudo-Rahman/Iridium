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
#include <QLineEdit>
#include <QLayout>
#include <QDialogButtonBox>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QSysInfo>
#include <QTextEdit>
#include <QThread>
#include <QScrollBar>
#include <ImagePreviewDialog.hpp>
#include <QStyledItemDelegate>
#include <boost/algorithm/string/join.hpp>
#include <Settings.hpp>

#include "IridiumApp.hpp"
#include "Utility/Utility.hpp"

using namespace iridium::rclone;

/**
 * @brief Classe permettant de définir la taille des items
 */
class MyItemDelegate : public QStyledItemDelegate
{
public:
	explicit MyItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

	[[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		return {30, 30};
	}

	void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
	{
		QStyledItemDelegate::initStyleOption(option, index);
		auto *model(static_cast<const RcloneFileModel *>(index.model()));
		if (model == nullptr)
			return;

		if (index == model->index(0, 0)) return;
		auto *item = dynamic_cast<TreeFileItem *>(model->itemFromIndex(index));
		if (item and item->getFile()->isDir() and index.column() == 0)
		{
			option->icon = Settings::dirIcon();
		}
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
	setIconSize(QSize(24, 24));
	setFont(QFont("Arial", 12));
	setAlternatingRowColors(true);
	setUniformRowHeights(false);
	setFocusPolicy(Qt::StrongFocus);

	header()->setSectionsMovable(true);
	header()->setFont(QFont("Arial", 11, QFont::Bold));
	header()->setSortIndicatorShown(true);
	header()->setSectionsClickable(true);
	header()->setStretchLastSection(false);
	setContextMenuPolicy(Qt::CustomContextMenu);

	_search_line_edit = new RoundedLineEdit(this);
	_search_line_edit->setClearButtonEnabled(false);
	_search_line_edit->hide();
	_search_line_edit->setPlaceholderText("Search");
	auto hide = _search_line_edit->addAction(style()->standardIcon(QStyle::SP_LineEditClearButton),
	                                         QLineEdit::LeadingPosition);
	_search_line_edit->setPlaceholderText("...");
	_search_line_edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(hide, &QAction::triggered, _search_line_edit, [this]()
	{
		_search_line_edit->clear();
		_search_line_edit->hide();
	});

	viewport()->stackUnder(_search_line_edit);
	_search_line_edit->raise();

	setItemDelegate(new MyItemDelegate(this));

	connectSignals();

	header()->setMinimumSectionSize(50);
	setColumnWidth(0, 0);

	setDragDropMode(DragDrop);
	setDropIndicatorShown(true);

	auto p = palette();
	// p.setColor(QPalette::HighlightedText, palette().color(QPalette::Text));
	// if other than macos change alternate base color
	if (QSysInfo::productType() not_eq "macos")
		p.setColor(QPalette::AlternateBase, palette().color(QPalette::Midlight));
	setPalette(p);
}

TreeFileView::TreeFileView(QWidget *parent) : QTreeView(parent) { initUI(); }

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
		for (int i = 0; i < _model->rowCount(index); i++)
		{
			auto child = _model->index(i, 0, index);
			collapse(child);
		}
	});

	connect(header(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize)
	{
		if (logicalIndex == 0 and newSize < QWidget::width() * .35)
			setColumnWidth(0, QWidget::width() * .35);
		if (logicalIndex == 2 and newSize < 120)
			setColumnWidth(2, 120);
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

	connect(_search_line_edit, &QLineEdit::textChanged, this, [this](const QString &text) { search(text); });

	connect(this, &TreeFileView::pathChanged, this, [this]()
	{
		_search_line_edit->clear();
		_search_line_edit->hide();
	});

	// when treeview is resized move search line edit
	connect(this, &TreeFileView::resized, this, [this]()
	{
		_search_line_edit->setFixedWidth(viewport()->width() * .4);
		if (viewport()->size().width() < _search_line_edit->size().width() * 1.1)
			_search_line_edit->hide();
		auto x_y = QPoint((viewport()->width() * .97) - _search_line_edit->width(),
		                  (viewport()->height() * .99) - _search_line_edit->height());
		_search_line_edit->move(x_y);
	});

	connect(this, &TreeFileView::previewed, this, [this](const QByteArray &data)
	{
		ImagePreviewDialog dialog(data, this);
		dialog.exec();
	});
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
	if (header()->sectionSize(0) < QWidget::width() * .35)
		setColumnWidth(0, QWidget::width() * .35);
	emit resized();
}

/**
 * @brief Back to previous folder
 */
void TreeFileView::back()
{
	if (not _model) return;
	if (_index_back.size() > 0)
	{
		auto index = _index_back.back().first;
		auto val = _index_back.back().second;
		_index_back.pop_back();
		_index_front.push_back(std::pair(rootIndex(), verticalScrollBar()->value()));
		QTreeView::setRootIndex(index);
		verticalScrollBar()->setValue(val);
	}
	else
		QTreeView::setRootIndex(_model->getRootIndex());
	emit pathChanged(getPath());
}

/**
 * @brief Go to next folder
 */
void TreeFileView::front()
{
	if (not _model) return;
	if (_index_front.size() > 0)
	{
		auto index = _index_front.back().first;
		auto val = _index_front.back().second;
		_index_front.pop_back();
		_index_back.push_back(std::pair(index.parent(), verticalScrollBar()->value()));
		QTreeView::setRootIndex(index);
		verticalScrollBar()->setValue(val);
	}
	emit pathChanged(getPath());
}

/**
 * @brief expand folder
 * @param index
 */
void TreeFileView::expand(const QModelIndex &index)
{
	if (not _model) return;

	auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));
	_model->setExpandOrDoubleClick(false);

	_model->addItem(item->getFile(), item);
	QTreeView::expand(index);
	selectionModel()->clearSelection();
}

/**
 * @brief double click on item
 * @param index
 */
void TreeFileView::doubleClick(const QModelIndex &index)
{
	if (not _model) return;

	auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));

	if (item == nullptr)
		return;

	if (not item->getFile()->isDir())
		return;

	search("");

	_model->setExpandOrDoubleClick(true);
	_model->addItem(item->getFile(), item);

	//     get parent index
	auto id = index.parent();
	_index_back.push_back(std::pair(id, verticalScrollBar()->value()));
	_index_front.clear();
	// if item getParent is null,it's first column item, else we get first column item.
	QTreeView::setRootIndex(item->siblingAtFirstColumn()->index());
	emit pathChanged(getPath());
	selectionModel()->clearSelection();
	verticalScrollBar()->setValue(0);
}

/**
 * @brief context menu
 * @param pos
 */
void TreeFileView::showContextMenu()
{
	if (_model == nullptr)
		return;
	ItemMenu menu(this);
	auto lisItem = getSelectedItems().empty() ? QList{getRootItem()} : getSelectedItems();
	if (not rootIndex().isValid())
	{
		if (QTreeView::selectedIndexes().isEmpty())
			return;

		menu.setActionEnabled(ItemMenu::Action::Copy, false,
		                      ItemMenu::Action::Delete, false,
		                      ItemMenu::Action::Sync, false);
	}
	if (lisItem.size() == 1)
		menu.setActionEnabled(ItemMenu::Action::Preview, Iridium::Utility::isPreviewable(*lisItem.first()->getFile()));
	else
		menu.setActionEnabled(ItemMenu::Action::Preview, false);

	if (QTreeView::selectedIndexes().isEmpty())
		menu.setActionEnabled(ItemMenu::Action::Copy, false,
		                      ItemMenu::Action::Delete, false,
		                      ItemMenu::Action::Tree, false,
		                      ItemMenu::Action::Sync, false);

	if (lisItem.size() > 1 or not lisItem.first()->getFile()->isDir())
		menu.setActionEnabled(ItemMenu::Action::Paste, false,
		                      ItemMenu::NewFolder, false,
		                      ItemMenu::Tree, false,
		                      ItemMenu::Sync, false);

	if (Iridium::Global::copy_files.empty())
		menu.setActionEnabled(ItemMenu::Paste, false);

	ItemMenu::Action action = menu.exec(QCursor::pos());

	switch (action)
	{
		case ItemMenu::Action::Info:
			for (auto item: lisItem)
			{
				auto *dialog = new ItemInfoDialog(item, this);
				dialog->show();
			}
			break;
		case ItemMenu::Action::Copy:
			Iridium::Global::clear_and_swap_copy_files(
				[lisItem]
				{
					std::vector<RcloneFilePtr> files;
					for (auto item: lisItem)
						files.push_back(item->getFile());
					return files;
				}());
			break;
		case ItemMenu::Action::Paste:
			copyto(Iridium::Global::copy_files);
			break;
		case ItemMenu::Action::Delete:
			{
				deleteFile(lisItem);
				break;
			}
		case ItemMenu::Action::NewFolder:
			mkdir();
			break;
		case ItemMenu::Action::Tree:
			{
				auto diag = new QDialog(this);
				diag->setMinimumSize(400, 400);
				diag->setWindowTitle(tr("Arborescence"));
				auto layout = new QVBoxLayout(diag);
				auto progress = new ProgressBar(ProgressBar::Circular, diag);
				progress->setRange(0, 0);
				layout->addWidget(progress, 0, Qt::AlignCenter);
				auto rclone = new ir::process();
				rclone->add_option(option::basic_option::uptr("--color", "NEVER"));
				auto file = lisItem.front()->getFile();
				rclone->on_finish([=,this,rclone = rclone](int exit)
				{
					IridiumApp::runOnMainThread([=]
					{
						progress->hide();
						auto *text = new QTextEdit(diag);
						text->setReadOnly(true);
						text->setWordWrapMode(QTextOption::NoWrap);
						text->setStyleSheet(
							"QTextEdit{background-color: #282c34; color: #abb2bf; border-radius: 25px; padding: 15px 10px;}");
						diag->layout()->addWidget(text);
						if (exit == 0)
							text->setText(boost::algorithm::join(rclone->get_output(), "\n").c_str());
					});
				});
				rclone->tree(*file);
				Iridium::Global::process_pool.add_process(rclone);
				diag->exec();
				if (rclone->is_running())
					rclone->stop();
			}
			break;
		case ItemMenu::Action::Sync:
			if (lisItem.size() == 1)
				Iridium::Global::sync_dirs.push_back(lisItem.front()->getFile());
			break;
		case ItemMenu::Action::Preview:
			preview(lisItem.front());
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
	emit pathChanged("");
	if (info == nullptr)
	{
		auto model = new QStandardItemModel(this);
		model->setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
		QTreeView::setModel(model);
		return;
	}

	if (_remote_info == info)
		return;

	_remote_info = nullptr;
	_remote_info = info;

	try
	{
		auto it = std::ranges::find_if(Iridium::Global::remote_model, [info](const auto &pair)
		{
			return *std::any_cast<RemoteInfoPtr>(pair.first) == *info;
		});

		if (it == Iridium::Global::remote_model.end())
		{
			if (!_remote_info->isLocal())
				_model = new RcloneFileModelDistant(_remote_info, this);
			else
				_model = new RcloneFileModelLocal(_remote_info, this);
			Iridium::Global::remote_model.emplace(info, _model);
		}
		else { _model = std::any_cast<RcloneFileModel *>(it->second); }
	}
	catch (std::exception &e) { qDebug() << e.what(); }

	QTreeView::setModel(_model);
}

/**
 * @brief return path of current folder
 * @return path
 */
QString TreeFileView::getPath() const
{
	auto index = QTreeView::rootIndex();
	if (!index.isValid())
		return "";
	auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));
	return item->getFile()->absolute_path().c_str();
}

/**
 * @brief paste items in current folder or in selected folder
 * @param items
 */
void TreeFileView::copyto(const std::vector<RcloneFilePtr> &files, TreeFileItem *paste)
{
	auto treePaste = paste == nullptr ? getRootItem() : paste;
	if (not treePaste->getFile()->isDir())
		return;
	for (const auto &file: files)
	{
		if (RcloneFileModel::fileInFolder(file, treePaste))
			continue;

		RcloneFilePtr newFile = std::make_shared<RcloneFile>(
				treePaste->getFile().get(),
				file->getName(),
				file->getSize(),
				file->isDir(),
				file->isDir() ? QDateTime::currentDateTime() : file->getModTime(),
				_remote_info
			);
		auto process = process_ptr(new ir::process());
		process->copy_to(*file, *newFile);
		process->on_finish([this, treePaste](int exit)
		{
			if (exit == 0)
				reload(treePaste);
		});
		process->on_stop([this,treePaste] { IridiumApp::runOnMainThread([this,treePaste] { reload(treePaste); }); });
		connectProcessreloadable(process.get());
		emit taskAdded(*file, *newFile, std::move(process), TaskRowParent::Copy);
	}
}

/**
 * @brief get selected items
 * @param can_be_empty
 * @return selected items or root item if can_be_empty is false
 */
QList<TreeFileItem *> TreeFileView::getSelectedItems()
{
	QList<TreeFileItem *> lst;
	for (int i = 0; i < QTreeView::selectedIndexes().length(); i = i + 4)
	{
		auto index = QTreeView::selectedIndexes().at(i);
		auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));
		if (index not_eq QTreeView::rootIndex() and item not_eq nullptr)
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
		showSearchLine();
		return;
	}

	// if ctrl + r for reload
	if (QKeySequence(event->modifiers() | event->key()).matches(Qt::CTRL + Qt::Key_R))
	{
		reloadRoot();
		return;
	}

	auto lisItem = getSelectedItems();

	if (lisItem.isEmpty() and getPath().isEmpty())
		return;
	if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Copy) and !lisItem.empty())
		Iridium::Global::clear_and_swap_copy_files(
			[lisItem]
			{
				std::vector<RcloneFilePtr> files;
				for (auto item: lisItem)
					files.push_back(item->getFile());
				return files;
			}());
	if (QKeySequence(event->modifiers() | event->key()).matches(QKeySequence::Paste) and getSelectedItems().size() == 1)
		copyto(Iridium::Global::copy_files);
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
					info->move(QCursor::pos());
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
			if (_clickIndex not_eq _model->index(0, 0))
				editItem(_clickIndex);
			break;
	}
}

void TreeFileView::keyReleaseEvent(QKeyEvent *event) { QTreeView::keyReleaseEvent(event); }

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
	                        tr("Suppression de ") + QString::number(files.size()) +
	                        (files.size() > 1 ? tr(" fichiers") : tr(" fichier")) + " ?", {}, this);

	msgb.addButton(tr("Supprimer"), QMessageBox::YesRole);
	auto cancel = msgb.addButton(tr("Annuler"), QMessageBox::NoRole);
	if (files.size() < 5) { msgb.setDetailedText(files.join("\n")); }
	msgb.setInformativeText(tr("Cette action est irréversible."));
	msgb.exec();
	if (msgb.clickedButton() == cancel)
		return;
	for (auto item: items)
	{
		auto process = process_ptr(new ir::process());
		connectProcessreloadable(process.get());
		process->on_finish([reloadItem = getRootItem(),this](int exit)
		{
			if (exit == 0)
				reload(reloadItem);
		});
		if (item->getFile()->isDir())
			process->purge(*item->getFile());
		else
			process->delete_file(*item->getFile());
		auto emptyFile = ire::file{};
		emptyFile.set_name("--");
		emit taskAdded(*item->getFile(), std::move(static_cast<RcloneFile>(emptyFile)), std::move(process),
		               TaskRowParent::Delete);
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
	_model->removeRow(item->row(), _model->indexFromItem(item).parent());
}

/**
 * @brief reload current folder or selected folders
 */
void TreeFileView::reload(TreeFileItem *treeItem)
{
	if (not _model or treeItem == nullptr) return;
	_model->reload(treeItem);
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
	auto *name = new RoundedLineEdit(dialog);
	name->setObjectName("name");
	name->setPlaceholderText(tr("Nom du dossier"));
	name->setClearButtonEnabled(true);
	name->setFocus();
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
	auto item = getSelectedItems().empty() ? getRootItem() : getSelectedItems().first();
	if (item == nullptr) return;
	auto rcloneFile = std::make_shared<RcloneFile>(
			item->getFile().get(),
			name,
			0,
			true,
			QDateTime::currentDateTime(),
			_remote_info
		);
	if (RcloneFileModel::fileInFolder(rcloneFile->getName(), item))
	{
		auto msgb = new QMessageBox(QMessageBox::Warning, tr("Création"), tr("Le dossier existe déjà"),
		                            QMessageBox::Ok, this);
		msgb->setModal(true);
		msgb->show();
		return;
	}
	auto process = process_ptr(new ir::process());
	connectProcessreloadable(process.get());
	process->mkdir(*rcloneFile);
	process->on_finish([=,process = process.get()](int exit)
	{
		if (exit == 0)
			reload(item);
		else
		{
			IridiumApp::runOnMainThread([=]
			{
				auto msgb = QMessageBox(QMessageBox::Critical, tr("Création"), tr("Le dossier n’a pas pu être créé"),
				                        QMessageBox::Ok, this);
				msgb.setDetailedText(process->get_error().back().c_str());
				msgb.exec();
			});
		}
	});
	auto file = RcloneFile(nullptr, "--", 0, true, QDateTime::currentDateTime(), nullptr);
	emit taskAdded(std::move(file), *rcloneFile, std::move(process), TaskRowParent::Mkdir);
}

/**
 * @brief edit item
 * @param index
 */
void TreeFileView::editItem(const QModelIndex &index)
{
	// if not the first column
	if (_model->index(index.row(), 0, index.parent()) not_eq index)
		return;
	auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));
	if (item == nullptr)
		return;
	// if not the first element of treeWidget
	if (!index.parent().isValid())
		return;

	openPersistentEditor(index);
	// focus on editor
	auto *editor = dynamic_cast<QLineEdit *>(indexWidget(index));
	auto before = editor->text();
	auto *validator = new QRegularExpressionValidator(QRegularExpression(R"([^\/:*?"<>|]*)"));
	QPalette p;
	p.setColor(QPalette::Highlight, QLineEdit().palette().highlight().color());
	// set highlight color
	editor->setPalette(p);
	editor->setValidator(validator);
	editor->setFocus();

	connect(editor, &QLineEdit::editingFinished, this, [before, editor, index, this]
	{
		_model->itemFromIndex(index)->setText(before);
		disconnect(editor, nullptr, nullptr, nullptr);
		this->closePersistentEditor(index);
		if (editor->text() == before or editor->text().isEmpty())
			return;
		if (not RcloneFileModel::fileInFolder(editor->text(),
		                                      dynamic_cast<TreeFileItem *>(_model->itemFromIndex(rootIndex()))))
			this->rename(dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index)), editor->text());
		else
		{
			auto msgb = QMessageBox(QMessageBox::Critical, tr("Renommage"), tr("Le fichier existe déjà"),
			                        QMessageBox::Ok, this);
			msgb.exec();
			auto item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));
			item->setText(item->getFile()->getName());
		}
	});
}

/**
 * @brief reanme item
 * @param item
 * @param newName
 */
void TreeFileView::rename(const TreeFileItem *item, const QString &newName)
{
	auto process = process_ptr(new ir::process());
	process->on_finish([=](int exit)
	{
		if (exit == 0)
			reload(dynamic_cast<TreeFileItem *>(item->parent()));
	});
	auto oldFile = *item->getFile();
	auto newFile = oldFile;
	newFile.setName(newName);
	process->move_to(oldFile, newFile);
	connectProcessreloadable(process.get());
	emit taskAdded(oldFile, newFile, std::move(process), TaskRowParent::Rename);
}

void TreeFileView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (QDateTime::currentMSecsSinceEpoch() - _clickTime < 700 and
		    QDateTime::currentMSecsSinceEpoch() - _clickTime > 350)
		{
			if (_clickIndex == indexAt(event->pos()))
			{
				auto index = indexAt(event->pos());
				if (index.isValid())
					editItem(index);
			}
		}
	}
	_clickIndex = indexAt(event->pos());
	_clickTime = QDateTime::currentMSecsSinceEpoch();
	QTreeView::mousePressEvent(event);
}

void TreeFileView::dragEnterEvent(QDragEnterEvent *event)
{
	if (getRootItem() == nullptr) return;
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
		_drag_sys_files = true;
	}
	event->accept();
}

/**
 * @brief drag enter event
 * @param event
 */
void TreeFileView::dragMoveEvent(QDragMoveEvent *event)
{
	_drag_border = true;
	auto tree = _drag_sys_files ? this : dynamic_cast<TreeFileView *>(event->source());

	auto not_possible = [this,event, tree]
	{
		event->ignore();
		tree->_dragable = false;
		setStyleSheet("");
		_drag_border = true;
	};
	if (not tree->rootIndex().isValid())
		return not_possible();

	auto index = indexAt(event->position().toPoint()).siblingAtColumn(0);
	// select all row
	if (index.isValid())
		selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	else
		selectionModel()->clearSelection();

	auto destination_item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index));

	if (destination_item == nullptr)
	{
		selectionModel()->clearSelection();
		destination_item = getRootItem();
	}

	if (destination_item == nullptr)
		return not_possible();

	if (destination_item->getFile()->isDir())
	{
		if (destination_item == getRootItem()) _drag_border = true;
		else
		{
			_drag_border = false;
			setStyleSheet("");
		}
	}
	else
	{
		destination_item = getRootItem();
		if (destination_item == nullptr)
			return not_possible();
		selectionModel()->clearSelection();
	}

	if (_drag_sys_files)
	{
		for (const auto &url: event->mimeData()->urls())
		{
			if (RcloneFileModel::fileInFolder(url.fileName(), destination_item))
				return not_possible();
		}
	}
	else
	{
		auto mimeData = qobject_cast<const TreeMimeData *>(event->mimeData());

		// not drop in itself
		if (not mimeData->items().isEmpty())
		{
			for (auto item: mimeData->items())
			{
				if (item->index() == index or
				    RcloneFileModel::fileInFolder(item->getFile()->getName(), destination_item))
					return not_possible();
			}
		}
	}

	if (styleSheet().isEmpty() and _drag_border)
	{
		setStyleSheet("QTreeView { border: 1px solid #2e86de; }");
		_drag_border = false;
	}

	_drop_destination = destination_item;
	tree->_dragable = true;
	event->accept();
}

/**
 * @brief drop event
 * @param event
 */
void TreeFileView::dropEvent(QDropEvent *event)
{
	setStyleSheet("");
	_drag_border = true;

	std::vector<RcloneFilePtr> files;
	auto *tree = _drag_sys_files ? this : dynamic_cast<TreeFileView *>(event->source());

	// get item destination
	auto destination_item = _drop_destination == nullptr ? getRootItem() : _drop_destination;

	if (_drag_sys_files)
	{
		if (not tree->_dragable)
		{
			event->ignore();
			return;
		}

		for (const auto &url: event->mimeData()->urls())
		{
			auto fileInfo = QFileInfo(url.toLocalFile());
			auto fileInfoParent = fileInfo.isDir() ? QFileInfo(fileInfo.dir().absolutePath()).dir() : fileInfo.dir();

			auto parent =
					std::make_shared<RcloneFile>(
							nullptr,
							fileInfoParent.absolutePath(),
							QFileInfo(fileInfoParent.absolutePath()).size(),
							true,
							QFileInfo(fileInfoParent.absolutePath()).lastModified(),
							nullptr
						);
			auto file = std::make_shared<RcloneFile>(
					parent->pointer(),
					fileInfo.isDir() ? fileInfo.dir().dirName() : fileInfo.fileName(),
					fileInfo.size(),
					fileInfo.isDir(),
					fileInfo.lastModified(),
					nullptr
				);
			_tmp_files.push_back(parent);
			files.push_back(file);
		}
	}
	else
	{
		auto tree = dynamic_cast<TreeFileView *>(event->source());
		if (not tree->_dragable)
		{
			event->ignore();
			return;
		}
		// get items to drop
		auto lst = qobject_cast<const TreeMimeData *>(event->mimeData())->items();
		if (lst.isEmpty())
		{
			event->ignore();
			return;
		}

		destination_item = destination_item->siblingAtFirstColumn();
		for (auto &item: lst)
			files.push_back(item->getFile());
	}

	_drag_sys_files = false;
	_drop_destination = nullptr;
	copyto(files, destination_item);
	event->acceptProposedAction();
}

void TreeFileView::dragLeaveEvent(QDragLeaveEvent *event)
{
	QTreeView::dragLeaveEvent(event);
	setStyleSheet("");
	_drag_border = true;
	_drag_sys_files = false;
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
	for (int i = 0; i < _model->itemFromIndex(QTreeView::rootIndex())->rowCount(); i++)
	{
		auto item = _model->itemFromIndex(QTreeView::rootIndex())->child(i);
		if (item->text().contains(text, Qt::CaseInsensitive))
			setRowHidden(i, QTreeView::rootIndex(), false);
		else
			setRowHidden(i, QTreeView::rootIndex(), true);
	}
}

bool TreeFileView::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::FocusOut:
			break;
		case QEvent::PaletteChange:
			break;
		default:
			break;
	}
	return QTreeView::event(event);
}

/**
 * @brief show search line edit for search item
 */
void TreeFileView::showSearchLine()
{
	_search_line_edit->clear();
	_search_line_edit->setFixedWidth(int(width() * .4));
	if (_search_line_edit->isHidden())
	{
		// move search line edit to bottom right
		auto x_y = QPoint(int(width() * .95) - _search_line_edit->width(),
		                  int(height() * .95) - _search_line_edit->height());
		_search_line_edit->move(x_y);
		_search_line_edit->show();
	}
	_search_line_edit->setFocus();
}

/**
 * @brief auto reload root folder every reload_time
 */
void TreeFileView::autoReload()
{
	while (true)
	{
		boost::this_thread::sleep_for(boost::chrono::seconds(Iridium::Global::reload_time));
		// get index
		reloadRoot();
	}
}

void TreeFileView::connectProcessreloadable(process *process) {}

TreeFileItem *TreeFileView::getRootItem() const
{
	auto index = rootIndex().siblingAtColumn(0);
	if (index.isValid())
		if (auto *item = dynamic_cast<TreeFileItem *>(_model->itemFromIndex(index)))
			return item;
	return nullptr;
}

void TreeFileView::reloadRoot()
{
	auto rootItem = getRootItem();
	if (rootItem not_eq nullptr)
		reload(rootItem);
}

void TreeFileView::preview(const TreeFileItem *item)
{
	if (item->getFile()->isDir())
		return;
	auto process = process_uptr(new ir::process());
	process->on_finish([rclone = process.get(),this](int exit)
	{
		if (exit not_eq 0)
			return;
		auto data = QByteArray::fromStdString(boost::algorithm::join(rclone->get_output(), "\n"));
		emit previewed(data);
	});
	process->cat(*item->getFile());
	Iridium::Global::process_pool.add_process(std::move(process));
}
