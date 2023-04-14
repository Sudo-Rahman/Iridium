//
// Created by sr-71 on 14/04/2023.
//

#ifndef IRIDIUM_MENUBAR_HPP
#define IRIDIUM_MENUBAR_HPP

#include <QMenuBar>

class MenuBar : public QMenuBar
{
Q_OBJECT

	QMenu *m_aboutMenu{};
	QAction *m_aboutAction{};

	QMenu *m_settingsMenu{};
	QAction *m_settingsAction{};

public:
	explicit MenuBar(QWidget *parent = nullptr);

private:
	void connectSignals();

};


#endif //IRIDIUM_MENUBAR_HPP
