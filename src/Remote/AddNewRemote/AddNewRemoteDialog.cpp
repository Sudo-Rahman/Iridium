//
// Created by sr-71 on 11/01/2023.
//

#include "AddNewRemoteDialog.hpp"
#include <QScrollArea>
#include "RemoteWidgetParam.hpp"

AddNewRemoteDialog::AddNewRemoteDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Ajout d’un remote"));
	layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 20, 5, 20);

	setMinimumSize(600, 500);

	auto *scrollArea = new QScrollArea(this);
	scrollArea->setStyleSheet("QScrollArea{border:none;}");
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setWidgetResizable(true);
	scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	scrollArea->setAlignment(Qt::AlignLeft);

	auto *scrollWidget = new QWidget(scrollArea);
	auto *scrollWidgetLayout = new QVBoxLayout(scrollWidget);
	scrollWidgetLayout->setAlignment(Qt::AlignTop);
	scrollArea->setWidget(scrollWidget);

	for (int t = Drive; t != Pcloud + 1; t++)
	{
		auto type = static_cast<RemoteType>(t);
		auto *widget = new RemoteWidgetParam(type);
		connect(widget, &RemoteWidgetParam::clicked, this, &AddNewRemoteDialog::changeParamsFrame);
		scrollWidgetLayout->addWidget(widget);
	}

	layout->addWidget(scrollArea);
	paramsFrame = findChildren<RemoteWidgetParam *>().first()->getParamsFrame();
	layout->addWidget(paramsFrame);

	for (auto *widget: scrollWidget->findChildren<RemoteWidgetParam *>())
	{
		widget->setMinimumWidth(200);
		connect(widget, &RemoteWidgetParam::newRemoteAdded, this, [this, widget]()
		{ emit newRemoteAdded(); });
	}

}

void AddNewRemoteDialog::changeParamsFrame(RemoteConfigParamsFrame *frame)
{
	frame->reset();
	if (frame == paramsFrame)
		return;
	if (paramsFrame not_eq nullptr)
	{
		layout->removeWidget(paramsFrame);
		paramsFrame->hide();
	}
	layout->addWidget(frame);
	paramsFrame = frame;
	paramsFrame->show();
}
