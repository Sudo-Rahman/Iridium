//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_MENUBAR_HPP
#define IRIDIUM_MENUBAR_HPP

#include <QMenuBar>

class MenuBar : public QMenuBar
{
Q_OBJECT

    QMenu *_about_menu{};
    QAction *_about_action{};

    QMenu *_settings_menu{};
    QAction *_settings_action{};

public:
    explicit MenuBar(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_MENUBAR_HPP
