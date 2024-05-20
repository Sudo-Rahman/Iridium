//
// Created by sr-71 on 20/05/2024.
//

#include "InfoWidget.hpp"

InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);
	_layout = new QHBoxLayout(this);
	_layout->setSizeConstraint(QLayout::SetFixedSize);
	_layout->setContentsMargins(0, 0, 0, 0);

	_label = new QLabel(tr("Tache en cours..."), this);
	_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_label->setFont(QFont("Arial", 10));

	_progress_bar = new CircularProgressBar(this);
	_progress_bar->setSize(20);

	_button = new RoundedButton(tr("Tout afficher"), this);
	_button->setFont(QFont("Arial", 10));
	auto textWidth = QFontMetrics(_button->font()).horizontalAdvance(_button->text());
	_button->setPadding(0);
	_button->setFixedSize(textWidth + 10, 20);
	_button->setRadius(10);
	_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	_button->adjustSize();

	connect(_button, &RoundedButton::clicked, [this]
	{
		_infos_widget_dialog->show(QCursor::pos());
	});

	_layout->addWidget(_label);
	_layout->addWidget(_progress_bar);
	_layout->addWidget(_button);

	_infos_widget_dialog = new InfoWidgetsDialog(this);

	hide();
}

void InfoWidget::addInfoWidget(QWidget *widget)
{
	_infos_widget_dialog->addWidget(widget);
	_infos_widgets.push_back(widget);
	show();
}

void InfoWidget::removeInfoWidget(QWidget *widget)
{
	_infos_widget_dialog->removeWidget(widget);
	std::erase(_infos_widgets, widget);
	widget->deleteLater();
	if (_infos_widgets.empty())
		hide();
}
