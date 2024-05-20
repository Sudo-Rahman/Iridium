//
// Created by sr-71 on 20/05/2024.
//

#pragma once

#include <QScrollArea>
#include <QHBoxLayout>

#include "AddNewRemoteDialog.hpp"

class InfoWidgetsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit InfoWidgetsDialog(QWidget *parent = nullptr);

	void addWidget(QWidget *widget);

	void removeWidget(QWidget *widget);

	void show(QPoint pos);

protected:
	void focusOutEvent(QFocusEvent *event) override;

private:
	QVBoxLayout *_layout{};
	QScrollArea *_scroll_area{};
	QWidget *_scroll_area_widget{};
};
