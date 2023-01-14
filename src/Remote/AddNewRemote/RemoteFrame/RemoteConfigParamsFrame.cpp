//
// Created by rahman on 13/01/23.
//

#include "RemoteConfigParamsFrame.hpp"

RemoteConfigParamsFrame::RemoteConfigParamsFrame(QWidget *parent) : QFrame(parent)
{
    layout = new QVBoxLayout(this);

    auto *tmpLayout = new QHBoxLayout;
    tmpLayout->addWidget(new QLabel(tr("Nom : "), this));

    remoteName = new QLineEdit(this);
    tmpLayout->addWidget(remoteName);

    layout->addLayout(tmpLayout);

}

void RemoteConfigParamsFrame::createUi()
{
    auto *tmpwidlay = new QHBoxLayout;

    logInBtn = new QPushButton(tr("Se connecter"), this);
    logInBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    tmpwidlay->addWidget(logInBtn, Qt::AlignBottom);
    tmpwidlay->setAlignment(logInBtn, Qt::AlignBottom);
    layout->addLayout(tmpwidlay);

    connect(logInBtn, &QPushButton::clicked, this, &RemoteConfigParamsFrame::addRemote);
}
