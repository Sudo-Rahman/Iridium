//
// Created by sr-71 on 29/03/2023.
//

#ifndef IRIDIUM_REMOTEWIDGET_HPP
#define IRIDIUM_REMOTEWIDGET_HPP

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include "../Remote.h"

class RemoteWidget : public QGroupBox
{
Q_OBJECT

private:
	QHBoxLayout *m_layout{};

	RemoteInfo m_remoteInfo{};

	bool m_hover{};

protected:
	void paintEvent(QPaintEvent *event) override;

	virtual bool event(QEvent *event) override;

public:
	explicit RemoteWidget(RemoteType type, Remote remote, const QString &name, QWidget *parent = nullptr);

	[[nodiscard]] const RemoteInfo &remoteInfo() const;

signals:

	void clicked(RemoteWidget *);
};


#endif //IRIDIUM_REMOTEWIDGET_HPP

