//
// Created by sr-71 on 29/03/2023.
//

#include "RemoteWidget.hpp"
#include "../../Config/Settings.hpp"

#include <QPainter>
#include <QEvent>
#include <QPropertyAnimation>
#include <utility>
#include <QGraphicsDropShadowEffect>

RemoteWidget::RemoteWidget(const RemoteInfo &remoteInfo, QWidget *parent) : QGroupBox(parent),
																			m_remoteInfo(std::move(
																				std::make_shared<RemoteInfo>(
																					remoteInfo)))
{
	init();
}

void RemoteWidget::paintEvent(QPaintEvent *event)
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

bool RemoteWidget::event(QEvent *event)
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
			emit clicked(this);
			break;
		default:
			break;
	}
	return QGroupBox::event(event);
}

void RemoteWidget::addBlur()
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

const RemoteInfoPtr &RemoteWidget::remoteInfo() const
{
	return m_remoteInfo;
}

void RemoteWidget::setSelected(uint8_t selected)
{
	if (!selected)
		m_selected->clear();
	else if (selected == 3)
		m_selected->setText("1-2");
	else
		m_selected->setText(QString::number(selected));
}

RemoteWidget::RemoteWidget(const RemoteInfoPtr &remoteInfo, QWidget *parent)
{
	m_remoteInfo = remoteInfo;
	init();
}

void RemoteWidget::init()
{
	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(10, 10, 10, 10);
	m_layout->setSpacing(15);

	auto *labelIcon = new QLabel;
	m_layout->addWidget(labelIcon);
// create pixmap
	QIcon icon;
	if (m_remoteInfo->isLocal())
	{
		icon = Settings::HARDDRIVE_ICON;
	} else
		icon = QIcon(QString::fromStdString(m_remoteInfo->m_icon));
	labelIcon->setPixmap(icon.pixmap(32, 32, QIcon::Normal, QIcon::On));


	auto *labelRemoteName = new QLabel(QString::fromStdString(m_remoteInfo->name()));
	// set auto size font
	QFont font = labelRemoteName->font();
	font.setPointSize(15);
	labelRemoteName->setFont(font);
	m_layout->addWidget(labelRemoteName);

	m_selected = new QLabel(this);
//	m_selected->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_selected->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	m_layout->addWidget(m_selected);
}
