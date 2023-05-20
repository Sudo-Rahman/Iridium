//
// Created by Rahman on 11/02/2023.
//

#ifndef IRIDIUM_ITEMMENU_HPP
#define IRIDIUM_ITEMMENU_HPP


#include <QMenu>
#include "TreeFileItem.hpp"

class ItemMenu : public QMenu
{
Q_OBJECT

private:
    QAction *_info{}, *_copy{}, *_paste{}, *_delete{}, *_new_folder{};

public:

    enum Action
    {
        None,
        Copy,
        Paste,
        Info,
        Delete,
        NewFolder
    };

    [[nodiscard]] Action action() const { return _action; }

    explicit ItemMenu(QWidget *parent = nullptr);

    void setActionEnabled(const QList<QPair<Action, bool>> &lst);

    // override exec and return action clicked
    Action exec(const QPoint &pos)
    {
        QMenu::exec(pos);
        return _action;
    }

private:
    Action _action{None};

signals:

    void copyed();

    void pasted();

    void info();

    void deleted();

    void newFolder();

};


#endif //IRIDIUM_ITEMMENU_HPP
