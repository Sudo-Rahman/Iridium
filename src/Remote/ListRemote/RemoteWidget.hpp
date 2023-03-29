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
protected:
	void paintEvent(QPaintEvent *event) override;

	virtual bool event(QEvent *event) override;


public:
	explicit RemoteWidget(RemoteType type,QString name, QWidget *parent = nullptr);

	[[nodiscard]] RemoteType getType() const;

	[[nodiscard]] QString getName() const;

private:
	QHBoxLayout *m_layout{};

	RemoteType m_type{};
	QString m_name{};

	bool m_hover{};
};


#endif //IRIDIUM_REMOTEWIDGET_HPP

