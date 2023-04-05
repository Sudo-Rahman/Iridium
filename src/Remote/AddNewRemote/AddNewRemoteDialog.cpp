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

	auto *tt = new RemoteWidgetParam(RemoteType::Drive);
	connect(tt, &RemoteWidgetParam::clicked, this, &AddNewRemoteDialog::changeParamsFrame);
	scrollWidgetLayout->addWidget(tt);

	auto *ttt = new RemoteWidgetParam(RemoteType::LocalHardDrive);
	connect(ttt, &RemoteWidgetParam::clicked, this, &AddNewRemoteDialog::changeParamsFrame);
	scrollWidgetLayout->addWidget(ttt);

	layout->addWidget(scrollArea);
	paramsFrame = tt->getParamsFrame();
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
