//
// Created by sr-71 on 29/03/2023.
//

#ifndef IRIDIUM_LISTREMOTEWIDGET_HPP
#define IRIDIUM_LISTREMOTEWIDGET_HPP

#include <QScrollArea>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QList>

#include "RemoteWidget.hpp"
#include <Remote.h>
#include <RoundedButton.hpp>
#include <RoundedLineEdit.hpp>
#include <Rclone.hpp>

struct remotes_selected
{
	RemoteWidget *first{};
	RemoteWidget *second{};
};

class ListRemoteWidget : public QScrollArea
{
Q_OBJECT

private:
	QVBoxLayout *m_layout{}, *m_remoteLayout{};

	RoundedLineEdit *m_recherche{};
	RoundedButton *m_add{};

	QList<RemoteWidget *> m_listRemote{};

	std::shared_ptr<remotes_selected> m_remoteSelected{};

	bool m_selected{};

	RclonesManager m_manager{};

	int m_width{};
	bool m_isExpand{};

public:
	explicit ListRemoteWidget(QWidget *parent = nullptr);

	void expand();

	const std::shared_ptr<remotes_selected> &remoteSelected()
	{ return m_remoteSelected; }

private:
	void getAllRemote();

	void searchRemote(const QString &name);

signals:

	void remoteClicked(const std::shared_ptr<remotes_selected> &);
};


#endif //IRIDIUM_LISTREMOTEWIDGET_HPP
