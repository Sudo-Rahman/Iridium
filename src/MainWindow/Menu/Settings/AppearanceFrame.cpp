//
// Created by sr-71 on 19/04/2023.
//

#include "AppearanceFrame.hpp"

AppearanceFrame::AppearanceFrame(QWidget *parent) : QFrame(parent)
{
	setMinimumWidth(400);
	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignTop);
	layout->setSpacing(10);
	layout->addWidget(new FolderColorGroupBox(this));
}
