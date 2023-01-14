//
// Created by sr-71 on 11/01/2023.
//

#include "ChoseRemoteDialog.hpp"
#include <QScrollArea>
#include "RemoteFrame/RemoteFrame.hpp"

ChoseRemoteDialog::ChoseRemoteDialog( QWidget * parent ) :  QDialog( parent )
{
    layout = new QHBoxLayout(this);

    setMinimumSize(500, 500);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    scrollArea->setAlignment(Qt::AlignLeft);

    auto *scrollWidget = new QWidget(scrollArea);
    scrollWidget->setContentsMargins(0,10,15,10);
    auto * scrollWidgetLayout = new QVBoxLayout(scrollWidget);
    scrollArea->setWidget(scrollWidget);

    auto *t = new RemoteFrame(RemoteType::Drive );
    connect(t, &RemoteFrame::clicked, this,& ChoseRemoteDialog::changeParamsFrame);
    scrollWidgetLayout->addWidget(t);

    for (int i = 0; i < 9; ++i)
    {
        auto *t = new RemoteFrame(RemoteType::Drive );
        connect(t, &RemoteFrame::clicked, this,& ChoseRemoteDialog::changeParamsFrame);
        scrollWidgetLayout->addWidget(t);
    }

    layout->addWidget(scrollArea);
    layout->addWidget(t->getParamsFrame());

}

void ChoseRemoteDialog::changeParamsFrame(RemoteConfigParamsFrame *frame)
{
    if(frame == paramsFrame)
        return;
    if(paramsFrame not_eq  nullptr){

        layout->removeWidget(paramsFrame);
        paramsFrame->hide();
    }
    layout->addWidget(frame);
    paramsFrame = frame;
    paramsFrame->show();
}
