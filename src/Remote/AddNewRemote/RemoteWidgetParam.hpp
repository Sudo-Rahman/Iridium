//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_REMOTEWIDGETPARAM_HPP
#define IRIDIUM_REMOTEWIDGETPARAM_HPP

#include <QGroupBox>
#include "../Remote.h"
#include "RemoteConfigParamsFrame.hpp"
#include "../../Rclone/Rclone.hpp"

class RemoteWidgetParam : public QGroupBox
{
Q_OBJECT
private:
	RemoteType type;
	QIcon m_icon{};
	bool m_hover{};
	RemoteConfigParamsFrame *paramsFrame{};

	void initParamsFrame();

public:
	explicit RemoteWidgetParam(RemoteType type, QWidget *parent = nullptr);

	[[nodiscard]] RemoteConfigParamsFrame *getParamsFrame() const;

protected:

	virtual bool event(QEvent *event) override;

	void paintEvent(QPaintEvent *event) override;

signals:

	void clicked(RemoteConfigParamsFrame *);
};


#endif //IRIDIUM_REMOTEWIDGETPARAM_HPP
