//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_MENUBAR_HPP
#define IRIDIUM_MENUBAR_HPP

#include <QMenuBar>

class MenuBar : public QMenuBar
{
Q_OBJECT

    QMenu *m_about_menu{};
    QAction *m_about_action{};

    QMenu *m_settings_menu{};
    QAction *m_settings_action{};

public:
    explicit MenuBar(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_MENUBAR_HPP
