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

	RemoteInfoPtr m_remoteInfo{};

	QLabel *m_selected{};

	bool m_hover{};

protected:
	void paintEvent(QPaintEvent *event) override;

	virtual bool event(QEvent *event) override;

public:
	explicit RemoteWidget(const RemoteInfo& remoteInfo, QWidget *parent = nullptr);

	[[nodiscard]] const RemoteInfoPtr &remoteInfo() const;

	void setSelected(uint8_t);

signals:

	void clicked(RemoteWidget *);
};


#endif //IRIDIUM_REMOTEWIDGET_HPP

