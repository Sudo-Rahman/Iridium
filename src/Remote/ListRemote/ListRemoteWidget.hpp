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
#include "../../Rclone/Rclone.hpp"
#include "../../Other/RoundedButton.hpp"
#include "../../Other/RoundedLineEdit.hpp"


class ListRemoteWidget : public QScrollArea
{
Q_OBJECT

private:
	QVBoxLayout *m_layout{}, *m_remoteLayout{};

	RoundedLineEdit *m_recherche{};
	RoundedButton *m_add{};

	QList<RemoteWidget *> m_listRemote{};

	RclonesManager m_manager{};

	int m_width{};
	bool m_isExpand{};

public:
	explicit ListRemoteWidget(QWidget *parent = nullptr);

	void expand();

private:
	void getAllRemote();

	void searchRemote(const QString &name);

signals:

	void remoteClicked(RemoteWidget *);
};


#endif //IRIDIUM_LISTREMOTEWIDGET_HPP
