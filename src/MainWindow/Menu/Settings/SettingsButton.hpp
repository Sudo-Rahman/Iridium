//
// Created by sr-71 on 19/04/2023.
//

#ifndef IRIDIUM_SETTINGSBUTTON_HPP
#define IRIDIUM_SETTINGSBUTTON_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>

class SettingsButton : public QGroupBox
{
Q_OBJECT

	QHBoxLayout *m_layout{};
	QLabel *m_label{};
	QFrame *m_frame{};

public:
	explicit SettingsButton(const QString &text, QWidget *parent = nullptr);

};


#endif //IRIDIUM_SETTINGSBUTTON_HPP
