//
// Created by sr-71 on 20/05/2024.
//

#pragma once
#include <QHBoxLayout>
#include <QLabel>

#include "CircularProgressBar.hpp"
#include "InfoWidgetsDialog.hpp"
#include "RoundedButton.hpp"


class InfoWidget : public  QWidget{
	Q_OBJECT

public:
	explicit InfoWidget(QWidget *parent = nullptr);

	void addInfoWidget(QWidget *widget);

	void removeInfoWidget(QWidget *widget);

	bool taskInProgress() const;

private:
	QHBoxLayout *_layout{};
	CircularProgressBar *_progress_bar{};
	QLabel *_label{};
	RoundedButton *_button{};

	InfoWidgetsDialog *_infos_widget_dialog{};
	std::vector<QWidget *> _infos_widgets{};
};
