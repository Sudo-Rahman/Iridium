//
// Created by sr-71 on 11/01/2023.
//

#include "AddNewRemoteDialog.hpp"
#include <QScrollArea>
#include "RemoteFrame/RemoteFrame.hpp"

AddNewRemoteDialog::AddNewRemoteDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Ajout d'un disque"));
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
	scrollArea->setWidget(scrollWidget);

	auto *t = new RemoteFrame(RemoteType::Drive);
	connect(t, &RemoteFrame::clicked, this, &AddNewRemoteDialog::changeParamsFrame);
	scrollWidgetLayout->addWidget(t);

	for (int i = 0; i < 9; ++i)
	{
		auto *t = new RemoteFrame(RemoteType::Drive);
		connect(t, &RemoteFrame::clicked, this, &AddNewRemoteDialog::changeParamsFrame);
		scrollWidgetLayout->addWidget(t);
	}

	layout->addWidget(scrollArea);
	paramsFrame = t->getParamsFrame();
	layout->addWidget(paramsFrame);

}

void AddNewRemoteDialog::changeParamsFrame(RemoteConfigParamsFrame *frame)
{
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
