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


class ListRemoteWidget : public QScrollArea
{
Q_OBJECT

private:
	QVBoxLayout *m_layout{};
	QVBoxLayout *m_remoteLayout{};

	QLineEdit *m_recherche{};
	QPushButton *m_add{};

	QList<RemoteWidget *> m_listRemote{};

	RclonesManager m_manager{};

public:
	explicit ListRemoteWidget(QWidget *parent = nullptr);

private:
	void getAllRemote();
	void searchRemote(const QString &name);
};


#endif //IRIDIUM_LISTREMOTEWIDGET_HPP
