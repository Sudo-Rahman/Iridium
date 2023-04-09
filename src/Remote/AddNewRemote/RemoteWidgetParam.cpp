//
// Created by sr-71 on 11/01/2023.
//

#include "RemoteWidgetParam.hpp"
#include "RemoteConfigParamsFrame.hpp"

#include "GoogleDrive/GoogleDriveRemoteConfigParamsFrame.hpp"
#include "Local/LocalRemoteConfigParamsFrame.hpp"
#include "Sftp/SftpRemoteConfigParamsFrame.hpp"
#include "../../Config/Settings.hpp"

#include <QLayout>
#include <QLabel>
#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <QGraphicsDropShadowEffect>

RemoteWidgetParam::RemoteWidgetParam(RemoteType type, QWidget *parent) :
	QGroupBox(parent), type(type)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setContentsMargins(10, 10, 10, 10);

	initParamsFrame();

	auto *layout = new QHBoxLayout(this);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layout->setSpacing(15);

	auto *labelIcon = new QLabel(this);
	layout->addWidget(labelIcon);


	auto *labelRemoteName = new QLabel(this);
	layout->addWidget(labelRemoteName);
	switch (RemoteWidgetParam::type)
	{
		case Drive:
			labelRemoteName->setText("Google Drive");
			break;
		case Sftp:
			labelRemoteName->setText("Sftp");
			break;
		case LocalHardDrive:
			labelRemoteName->setText("Local");
			break;
		default:
			break;
	}
	if (type == RemoteType::LocalHardDrive)
		m_icon = Settings::HARDDRIVE_ICON;
	else
		m_icon = QIcon(QString::fromStdString(remoteIco.find(RemoteWidgetParam::type)->second));
	labelIcon->setPixmap(m_icon.pixmap(32, 32, QIcon::Normal, QIcon::On));

	update();
}

void RemoteWidgetParam::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(QGroupBox::palette().color(QPalette::Light));
	m_click ? painter.setBrush(QGroupBox::palette().color(QPalette::Window)) : painter.setBrush(
		QGroupBox::palette().color(QPalette::Light));

	// draw rounded rect
	QRect rect = this->rect().marginsRemoved(QMargins(5, 5, 5, 5));
	rect.setWidth(rect.width() - 1);
	rect.setHeight(rect.height() - 1);
	painter.drawRoundedRect(rect, 10, 10);
}

void RemoteWidgetParam::initParamsFrame()
{
	switch (type)
	{
		case Drive:
			paramsFrame = new GoogleDriveRemoteConfigParamsFrame();
			break;
		case Sftp:
			paramsFrame = new SftpRemoteConfigParamsFrame();
			break;
		case LocalHardDrive:
			paramsFrame = new LocalRemoteConfigParamsFrame();
			break;
	}
	connect(paramsFrame, &RemoteConfigParamsFrame::remoteAdded, this, &RemoteWidgetParam::newRemoteAdded);
}

RemoteConfigParamsFrame *RemoteWidgetParam::getParamsFrame() const
{
	return paramsFrame;
}

bool RemoteWidgetParam::event(QEvent *event)
{
	switch (event->type())
	{
		// mouse hover repaint
		case QEvent::Enter:
			m_hover = true;
			addBlur();
			break;
		case QEvent::Leave:
			m_hover = false;
			addBlur();
			break;
		case QEvent::MouseButtonPress:
			// change cursor
			m_click = true;
			repaint();
			setCursor(Qt::PointingHandCursor);
			break;
		case QEvent::MouseButtonRelease:
			// change cursor
			m_click = false;
			repaint();
			setCursor(Qt::ArrowCursor);
			emit clicked(paramsFrame);
			break;
		default:
			break;
	}
	return QGroupBox::event(event);
}

void RemoteWidgetParam::addBlur()
{
	// if not hover remove effect
	if (!m_hover)
	{
		this->setGraphicsEffect(nullptr);
		return;
	}
	// resize effect
	auto effect = new QGraphicsDropShadowEffect(this);
	effect->setBlurRadius(20);
	effect->setOffset(0, 0);
	effect->setColor(QWidget::palette().color(QPalette::Dark));
	this->setGraphicsEffect(effect);
}
