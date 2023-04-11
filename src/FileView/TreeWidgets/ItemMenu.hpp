//
// Created by sr-71 on 11/02/2023.
//

#ifndef IRIDIUM_ITEMMENU_HPP
#define IRIDIUM_ITEMMENU_HPP


#include <QMenu>
#include "TreeFileItem.hpp"

class ItemMenu : public QMenu
{
Q_OBJECT

private:
	QAction *m_info{};
	QAction *m_copy{};
	QAction *m_paste{};
	QAction *m_delete{};
	QAction *m_newFolder{};


public:

	enum Action{
		Copy,
		Paste,
		Info,
		Delete,
		NewFolder
	};
	explicit ItemMenu(QWidget *parent = nullptr);

	void setActionEnabled(const QList<QPair<Action, bool>> &lst);

signals:

	void copyed();

	void pasted();

	void info();

	void deleted();

	void newFolder();

};


#endif //IRIDIUM_ITEMMENU_HPP
