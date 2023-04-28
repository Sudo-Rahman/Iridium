//
// Created by sr-71 on 29/03/2023.
//

#ifndef IRIDIUM_REMOTEWIDGET_HPP
#define IRIDIUM_REMOTEWIDGET_HPP

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <Remote.h>
#include <RoundedButton.hpp>

class RemoteWidget : public QGroupBox
{
Q_OBJECT

protected:
	QHBoxLayout *m_layout{};

	RemoteInfoPtr m_remoteInfo{};
	QLabel *m_selected{};

	bool m_hover{};
	bool m_click{};

    bool deletable = true;

protected:
	void paintEvent(QPaintEvent *event) override;

	virtual bool event(QEvent *event) override;

private:
	void init();

	void addBlur();

public:
	explicit RemoteWidget(const RemoteInfo &remoteInfo, bool deletable = true,QWidget *parent = nullptr);

	explicit RemoteWidget(const RemoteInfoPtr &remoteInfo,bool deletable = true, QWidget *parent = nullptr);

	[[nodiscard]] const RemoteInfoPtr &remoteInfo() const;

	void setSelectedText(const QString &text);

signals:

	void clicked(RemoteWidget *);

	void deleted(RemoteWidget *);
};


#endif //IRIDIUM_REMOTEWIDGET_HPP

