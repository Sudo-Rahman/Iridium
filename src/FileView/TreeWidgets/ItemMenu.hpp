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
    QAction *_info{}, *_copy{}, *_paste{}, *_delete{}, *_new_folder{}, *_tree{},*_sync{};

public:

    enum Action
    {
        None,
        Copy,
        Paste,
        Info,
        Delete,
        NewFolder,
        Tree,
        Sync
    };

    [[nodiscard]] Action action() const { return _action; }

    explicit ItemMenu(QWidget *parent = nullptr);

    void setActionEnabled(const Action &action, bool enabled);

    template<class ...Args>
    void setActionEnabled(const Action &action, bool enabled, Args &&... args)
    {
        setActionEnabled(action, enabled);
        setActionEnabled(std::forward<Args>(args)...);
    }

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

    void tree();

    void sync();

};


#endif //IRIDIUM_ITEMMENU_HPP
