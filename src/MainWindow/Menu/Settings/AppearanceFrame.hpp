//
// Created by sr-71 on 19/04/2023.
//

#ifndef IRIDIUM_APPEARANCEFRAME_HPP
#define IRIDIUM_APPEARANCEFRAME_HPP

#include <QFrame>
#include <QLabel>
#include "FolderColorGroupBox.hpp"


class AppearanceFrame : public QFrame
{
Q_OBJECT

public:
	explicit AppearanceFrame(QWidget *parent = nullptr);

};


#endif //IRIDIUM_APPEARANCEFRAME_HPP
