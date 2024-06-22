//
// Created by Rahman on 11/01/2023.
//

#include "AddNewRemoteDialog.hpp"
#include <QScrollArea>
#include "RemoteWidgetParam.hpp"

AddNewRemoteDialog::AddNewRemoteDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Ajout remote"));
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(5, 20, 5, 20);

    setMinimumSize(600, 500);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    scrollArea->setAlignment(Qt::AlignLeft);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *scrollWidget = new QWidget(scrollArea);
    auto *scrollWidgetLayout = new QVBoxLayout(scrollWidget);
    scrollWidgetLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(scrollWidget);


    auto *local = new RemoteWidgetParam(ire::remote::remote_type::none, this);
    connect(local, &RemoteWidgetParam::clicked, this, [this, local]
    {
        for (auto *wid: findChildren<RemoteWidgetParam *>())
            wid->unselect();
        local->select();
        changeParamsFrame(local->getParamsFrame());
        local->getParamsFrame()->focusLineEdit();
    });
    scrollWidgetLayout->addWidget(local);

    for (int t = 0; t != ire::remote::remote_type::other; t++)
    {
        auto type = static_cast<ire::remote::remote_type>(t);
        auto *widget = new RemoteWidgetParam(type,this);
        connect(widget, &RemoteWidgetParam::clicked, this, [this, widget]
        {
            for (auto *wid: findChildren<RemoteWidgetParam *>())
                wid->unselect();
            widget->select();
            changeParamsFrame(widget->getParamsFrame());
            widget->getParamsFrame()->focusLineEdit();
        });
        scrollWidgetLayout->addWidget(widget);
    }

    _layout->addWidget(scrollArea);
    _paramsFrame = findChildren<RemoteWidgetParam *>().first()->getParamsFrame();
    findChildren<RemoteWidgetParam *>().first()->select();
    _layout->addWidget(_paramsFrame);

    for (auto *widget: scrollWidget->findChildren<RemoteWidgetParam *>())
    {
        widget->setMinimumWidth(200);
        connect(widget, &RemoteWidgetParam::newRemoteAdded, this, [this]() { emit newRemoteAdded(); });
    }
    findChildren<RemoteWidgetParam *>().first()->getParamsFrame()->focusLineEdit();
}

void AddNewRemoteDialog::changeParamsFrame(RemoteConfigParamsFrame *frame)
{
    frame->reset();
    if (frame == _paramsFrame)
        return;
    if (_paramsFrame not_eq nullptr)
    {
        _layout->removeWidget(_paramsFrame);
        _paramsFrame->hide();
    }
    _layout->addWidget(frame);
    _paramsFrame = frame;
    _paramsFrame->show();
}
