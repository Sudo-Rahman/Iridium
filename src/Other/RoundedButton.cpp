//
// Created by Rahman on 08/04/2023.
//

#include "RoundedButton.hpp"
#include <QEvent>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <qguiapplication.h>

#include "IridiumApp.hpp"

const QString lightMode = "QPushButton {"
		"border: none;"
		"color: black;"
		"background-color: transparent;"
		"height: {height}px;"
		"width: {width}px;"
		"padding: {padding}px;"
		"border-radius: {radius}px;"
		"}"
		"QPushButton:hover {"
		"background-color: rgba(200, 200, 200, 0.4);"
		"}"
		"QPushButton:pressed {"
		"background-color: rgba(200, 200, 200, 0.6);"
		"}";

const QString darkMode = "QPushButton {"
		"border: none;"
		"color: white;"
		"background-color: transparent;"
		"height: {height}px;"
		"width: {width}px;"
		"padding: {padding}px;"
		"border-radius: {radius}px;"
		"}"
		"QPushButton:hover {"
		"background-color: rgba(110, 110, 110, 0.6);"
		"}"
		"QPushButton:pressed {"
		"background-color: rgba(110, 110, 110, 1);"
		"}";

QString RoundedButton::getStyleSheet()
{
	auto style = qApp->property("dark").toBool() ? darkMode : lightMode;

	style.replace("{height}", QString::number(height()));
	style.replace("{width}", QString::number(width()));
	style.replace("{padding}", QString::number(_padding));
	style.replace("{radius}", QString::number(_circular ? height() / 2 : _radius));
	return style;
}

/**
 * @brief Construct a new Tree File View Rounded Button:: Tree File View Rounded Button object
 * @param text
 * @param parent
 */
RoundedButton::RoundedButton(const QString &text, QWidget *parent)
	: QPushButton(text, parent)
{
	// size font to fit button
	QFont font = this->font();
	font.setPixelSize(17);
	setFont(font);
	setStyleSheet(getStyleSheet());
	IridiumApp::onThemeChange.connect([this] { setStyleSheet(getStyleSheet()); });
}

RoundedButton::RoundedButton(const QIcon &icon, QWidget *parent)
	: QPushButton(parent)
{
	setIcon(icon);
	// size font to fit button
	QFont font = this->font();
	font.setPixelSize(17);
	setFont(font);
	setStyleSheet(getStyleSheet());
	IridiumApp::onThemeChange.connect([this] { setStyleSheet(getStyleSheet()); });
}
