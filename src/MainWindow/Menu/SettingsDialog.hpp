//
// Created by sr-71 on 16/04/2023.
//

#ifndef IRIDIUM_SETTINGSDIALOG_HPP
#define IRIDIUM_SETTINGSDIALOG_HPP

#include <QDialog>
#include <QScrollArea>
#include <QLayout>

class SettingsDialog : public QDialog
{
Q_OBJECT

	QScrollArea *m_leftScrollArea;
	QWidget *m_leftScrollAreaWidget;
	QVBoxLayout *m_leftScrollAreaLayout;

	QScrollArea *m_rightScrollArea;
	QWidget *m_rightScrollAreaWidget;
	QVBoxLayout *m_rightScrollAreaLayout;


public:
	explicit SettingsDialog(QWidget *parent = nullptr);

};


#endif //IRIDIUM_SETTINGSDIALOG_HPP
