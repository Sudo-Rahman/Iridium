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

    // add icons
    _info->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    _copy->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    _paste->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    _delete->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    _new_folder->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));

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

}


void ItemMenu::setActionEnabled(const QList<QPair<Action, bool>> &lst)
{
    for (const auto &pair: lst)
    {
        switch (pair.first)
        {
            case Copy:
                _copy->setEnabled(pair.second);
                break;
            case Paste:
                _paste->setEnabled(pair.second);
                break;
            case Info:
                _info->setEnabled(pair.second);
                break;
            case Delete:
                _delete->setEnabled(pair.second);
                break;
            case NewFolder:
                _new_folder->setEnabled(pair.second);
                break;
            case None:
            default:
                break;
        }
    }

}
