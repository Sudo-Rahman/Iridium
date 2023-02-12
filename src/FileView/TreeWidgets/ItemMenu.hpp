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


public:
	explicit ItemMenu(QWidget *parent = nullptr);

	void m_info_click(TreeFileItem *file);

signals:

	void copyed();

	void pasted();

	void info();

};


#endif //IRIDIUM_ITEMMENU_HPP
