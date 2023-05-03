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

    m_info = addAction(tr("Information"), this, [this]
    {
        m_action = Info;
        emit info();
    });
    m_copy = addAction(tr("Copier"), this, [this]
    {
        m_action = Copy;
        emit copyed();
    });
    m_paste = addAction(tr("Coller"), this, [this]
    {
        m_action = Paste;
        emit pasted();
    });
    m_delete = addAction(tr("Supprimer"), this, [this]
    {
        m_action = Delete;
        emit deleted();
    });
    m_new_folder = addAction(tr("Nouveau dossier"), this, [this]
    {
        m_action = NewFolder;
        emit newFolder();
    });

    // add icons
    m_info->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    m_copy->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_paste->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_delete->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    m_new_folder->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));

    // show shortcuts
    m_info->setShortcut(Qt::Key_Space);
    m_info->setShortcutVisibleInContextMenu(true);
    m_copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    m_copy->setShortcutVisibleInContextMenu(true);
    m_paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    m_paste->setShortcutVisibleInContextMenu(true);
    m_delete->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete));
    m_delete->setShortcutVisibleInContextMenu(true);
    m_new_folder->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    m_new_folder->setShortcutVisibleInContextMenu(true);

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
                m_new_folder->setEnabled(pair.second);
                break;
        }
    }

}
