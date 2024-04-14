//
// Created by Rahman on 11/02/2023.
//

#include <QDialog>
#include <QLayout>
#include <QStyle>
#include "ItemMenu.hpp"
#include "ItemInfoDialog.hpp"

ItemMenu::ItemMenu(QWidget *parent) : QMenu(parent)
{

    _info = addAction(tr("Information"), this, [this]
    {
        _action = Info;
        emit info();
    });
    _copy = addAction(tr("Copier"), this, [this]
    {
        _action = Copy;
        emit copyed();
    });
    _paste = addAction(tr("Coller"), this, [this]
    {
        _action = Paste;
        emit pasted();
    });
    _delete = addAction(tr("Supprimer"), this, [this]
    {
        _action = Delete;
        emit deleted();
    });
    _new_folder = addAction(tr("Nouveau dossier"), this, [this]
    {
        _action = NewFolder;
        emit newFolder();
    });
    _tree = addAction(tr("Arbre"), this, [this]
    {
        _action = Tree;
        emit tree();
    });

    // _sync = addAction(tr("Synchroniser"), this, [this]
    // {
    //     _action = Sync;
    //     emit sync();
    // });
    _preview = addAction(tr("Aperçu"), this, [this]
    {
        _action = Preview;
        emit preview();
    });

    // add icons
    _info->setIcon(QIcon(":/ressources/information.png"));
    _copy->setIcon(QIcon(":/ressources/copy.png"));
    _paste->setIcon(QIcon(":/ressources/paste.png"));
    _delete->setIcon(QIcon(":/ressources/delete.png"));
    _new_folder->setIcon(QIcon(":/ressources/new-folder.png"));
    _tree->setIcon(QIcon(":/ressources/tree.png"));
    // _sync->setIcon(QIcon(":/ressources/sync-cloud.png"));
    _preview->setIcon(QIcon(":/ressources/preview.png"));

    // show shortcuts
    _info->setShortcut(Qt::Key_Space);
    _info->setShortcutVisibleInContextMenu(true);
    _copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    _copy->setShortcutVisibleInContextMenu(true);
    _paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    _paste->setShortcutVisibleInContextMenu(true);
    _delete->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete));
    _delete->setShortcutVisibleInContextMenu(true);
    _new_folder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    _new_folder->setShortcutVisibleInContextMenu(true);
    _tree->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));

}


void ItemMenu::setActionEnabled(const ItemMenu::Action &action, bool enabled)
{
    switch (action)
    {
        case Copy:
            _copy->setEnabled(enabled);
            break;
        case Paste:
            _paste->setEnabled(enabled);
            break;
        case Info:
            _info->setEnabled(enabled);
            break;
        case Delete:
            _delete->setEnabled(enabled);
            break;
        case NewFolder:
            _new_folder->setEnabled(enabled);
            break;
        case Tree:
            _tree->setEnabled(enabled);
            break;
        case Sync:
            // _sync->setEnabled(enabled);
            break;
        case Preview:
            _preview->setEnabled(enabled);
            break;
        case None:
        default:
            break;
    }
}
