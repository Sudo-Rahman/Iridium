//
// Created by sr-71 on 11/02/2023.
//

#include <QDialog>
#include <QLayout>
#include <QStyle>
#include "ItemMenu.hpp"
#include "ItemInfoDialog.hpp"

ItemMenu::ItemMenu(QWidget *parent) : QMenu(parent)
{

    m_info = addAction(tr("Information"), this, &ItemMenu::info);
    m_copy = addAction(tr("Copy"), this, &ItemMenu::copyed);
    m_paste = addAction(tr("Coller"), this, &ItemMenu::pasted);
    m_delete = addAction(tr("Supprimer"), this, &ItemMenu::deleted);
    m_newFolder = addAction(tr("Nouveau dossier"), this, &ItemMenu::newFolder);

    // add icons
    m_info->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    m_copy->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_paste->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_delete->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    m_newFolder->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));

    // show shortcuts
    m_info->setShortcut(Qt::Key_Space);
    m_info->setShortcutVisibleInContextMenu(true);
    m_copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    m_copy->setShortcutVisibleInContextMenu(true);
    m_paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    m_paste->setShortcutVisibleInContextMenu(true);
    m_delete->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete));
    m_delete->setShortcutVisibleInContextMenu(true);
    m_newFolder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    m_newFolder->setShortcutVisibleInContextMenu(true);

}


void ItemMenu::setActionEnabled(const QList<QPair<Action, bool>> &lst)
{
    for (const auto &pair: lst)
    {
        switch (pair.first)
        {
            case Copy:
                m_copy->setEnabled(pair.second);
                break;
            case Paste:
                m_paste->setEnabled(pair.second);
                break;
            case Info:
                m_info->setEnabled(pair.second);
                break;
            case Delete:
                m_delete->setEnabled(pair.second);
                break;
            case NewFolder:
                m_newFolder->setEnabled(pair.second);
                break;
        }
    }

}
