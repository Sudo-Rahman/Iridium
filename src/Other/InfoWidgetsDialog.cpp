//
// Created by sr-71 on 20/05/2024.
//

#include "InfoWidgetsDialog.hpp"
#include <QEvent>

InfoWidgetsDialog::InfoWidgetsDialog(QWidget *parent) : QDialog(parent),
                                                        _scroll_area(new QScrollArea(this)),
                                                        _scroll_area_widget(new QWidget)
{
	setWindowTitle(tr("Tache en cours"));
	setContentsMargins(0, 0, 0, 0);

	// only show the close button
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
	setFixedSize(250, 150);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	hide();

	_scroll_area_widget->setContentsMargins(0, 0, 0, 0);

	_layout = new QVBoxLayout(_scroll_area_widget);
	_layout->setContentsMargins(0, 0, 0, 0);

	_scroll_area->setWidget(_scroll_area_widget);
	_scroll_area->setContentsMargins(0, 0, 0, 0);
	_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_scroll_area->setFrameShape(QFrame::NoFrame);
	_scroll_area->setWidgetResizable(true);

	_layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);


	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(_scroll_area);
}

void InfoWidgetsDialog::addWidget(QWidget *widget) { _layout->addWidget(widget); }

void InfoWidgetsDialog::removeWidget(QWidget *widget)
{
	_layout->removeWidget(widget);
	widget->deleteLater();
}

void InfoWidgetsDialog::show(QPoint pos)
{
	move(pos - QPoint(width() / 2, height() + 20));
	setVisible(true);
	setFocus();
}

void InfoWidgetsDialog::focusOutEvent(QFocusEvent *event)
{
	setVisible(false);
	QDialog::focusOutEvent(event);
}
