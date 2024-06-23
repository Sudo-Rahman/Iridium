//
// Created by Rahman on 11/02/2023.
//

#include <QDialog>
#include <QLayout>
#include <QStyle>
#include "ItemMenu.hpp"
#include "ItemInfoDialog.hpp"

ItemMenu::ItemMenu(const QList<TreeFileItem *> &files, QWidget *parent) : QMenu(parent)
{
	bool copy_, delete_;
	copy_ = delete_ = true;
	if (files.size() == 1)
	{
		if (files.first() == nullptr)
			return;

		auto file = files.first()->getFile();

		if (file->is_dir())
		{
			_paste = new QAction(tr("Coller"), this);
			connect(_paste, &QAction::triggered, this, [this]
			{
				_action = Paste;
				emit pasted();
			});
			_paste->setIcon(QIcon(":/resources/paste.png"));
			_paste->setShortcut(QKeySequence::Paste);

			_new_folder = new QAction(tr("Nouveau dossier"), this);
			connect(_new_folder, &QAction::triggered, this, [this]
			{
				_action = NewFolder;
				emit newFolder();
			});
			_new_folder->setIcon(QIcon(":/resources/new-folder.png"));
			_new_folder->setShortcut(QKeySequence::New);

			_tree = new QAction(tr("Arbre"), this);
			connect(_tree, &QAction::triggered, this, [this]
			{
				_action = Tree;
				emit tree();
			});
			_tree->setIcon(QIcon(":/resources/tree.png"));
			_tree->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));

			if (file->parent() == nullptr)
			{
				if (!file->getRemoteInfo()->isLocal())
				{
					_clean_trash = new QAction(tr("Vider la corbeille"), this);
					connect(_clean_trash, &QAction::triggered, this, [this]
					{
						_action = CleanTrash;
						emit trashCleaned();
					});
					_clean_trash->setIcon(QIcon(":/resources/clean.png"));
				}

				// if root file
				copy_ = delete_ = false;
			}

			_sync = new QAction(tr("Synchroniser"), this);
			connect(_sync, &QAction::triggered, this, [this]
			{
				_action = Sync;
				emit sync();
			});
			_sync->setIcon(QIcon(":/resources/sync-cloud.png"));
		}
	}
	if (!files.empty())
	{
		_info = new QAction(tr("Information"), this);
		connect(_info, &QAction::triggered, this, [this]
		{
			_action = Info;
			emit info();
		});
		_info->setIcon(QIcon(":/resources/information.png"));
		_info->setShortcut(Qt::Key_Space);

		if (copy_)
		{
			_copy = new QAction(tr("Copier"), this);
			connect(_copy, &QAction::triggered, this, [this]
			{
				_action = Copy;
				emit copied();
			});
			_copy->setIcon(QIcon(":/resources/copy.png"));
			_copy->setShortcut(QKeySequence::Copy);
		}

		if (delete_)
		{
			_delete = new QAction(tr("Supprimer"), this);
			connect(_delete, &QAction::triggered, this, [this]
			{
				_action = Delete;
				emit deleted();
			});
			_delete->setIcon(QIcon(":/resources/delete.png"));
			_delete->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Delete));
		}
	}

	for (const auto &action: {_info, _copy, _paste, _delete, _new_folder, _tree, _sync, _clean_trash})
	{
		if (action)
		{
			addAction(action);
			action->setShortcutVisibleInContextMenu(true);
		}
	}
}

void ItemMenu::setActionEnabled(const Action &action, bool enabled)
{
	switch (action)
	{
		case Copy:
			if (!enabled) removeAction(_copy);
			break;
		case Paste:
			if (!enabled) removeAction(_paste);
			break;
		case Info:
			if (!enabled) removeAction(_info);
			break;
		case Delete:
			if (!enabled) removeAction(_delete);
			break;
		case NewFolder:
			if (!enabled) removeAction(_new_folder);
			break;
		case Tree:
			if (!enabled) removeAction(_tree);
			break;
		case Sync:
			if (!enabled) removeAction(_sync);
			break;
		case CleanTrash:
			if (!enabled) removeAction(_clean_trash);
			break;
		case None:
		default:
			break;
	}
}
