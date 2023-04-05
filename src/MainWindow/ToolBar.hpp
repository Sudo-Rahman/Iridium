//
// Created by sr-71 on 05/04/2023.
//

#ifndef IRIDIUM_TOOLBAR_HPP
#define IRIDIUM_TOOLBAR_HPP

#include <QToolBar>
#include <QAction>

class ToolBar : public QToolBar
{
Q_OBJECT

	QAction *m_hideListeRemotes{};
	bool m_isHideListeRemotes{};

public:
	explicit ToolBar(QWidget *parent = nullptr);

private:
	void connectSignals();

signals:

	void hideListeRemotes(bool);

};


#endif //IRIDIUM_TOOLBAR_HPP
