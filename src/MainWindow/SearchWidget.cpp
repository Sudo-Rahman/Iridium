//
// Created by sr-71 on 08/05/2023.
//
#include <QFormLayout>

#include "SearchWidget.hpp"
#include "Config/Settings.hpp"
#include <QPainter>
#include <QEvent>
#include <QLabel>
#include <QStyledItemDelegate>
#include <QListView>
#include <iridium/options.hpp>

#include "CircularProgressBar.hpp"

using namespace iridium::rclone::option;

class CustomStyledItemDelegate : public QStyledItemDelegate
{
protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
	           const QModelIndex &index) const override
	{
		auto &refToNonConstOption = const_cast<QStyleOptionViewItem &>(option);
		refToNonConstOption.showDecorationSelected = false;
		//refToNonConstOption.state &= ~QStyle::State_HasFocus & ~QStyle::State_MouseOver;

		QStyledItemDelegate::paint(painter, refToNonConstOption, index);
	}
};

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
	_filter_search = new FilterGroupBox(tr("Filtres (non disponible pour la recherche locale)"), this);

	_progressBar = new LinearProgressBar(this);
	setFocusPolicy(Qt::StrongFocus);
	_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_progressBar->setFixedHeight(12);

	auto model = new QStandardItemModel(this);
	_remotes_comboBox = new QComboBox(this);
	_remotes_comboBox->setModel(model);
	_remotes_comboBox->setItemDelegate(new CustomStyledItemDelegate());
	// set 5 items visible
	auto view = new QListView();
	_remotes_comboBox->setView(view);
	view->setMinimumWidth(150);
	_remotes_comboBox->setMaxVisibleItems(3);

	_search = new RoundedLineEdit(this);
	_search->setPlaceholderText(".png");
	_start = new QPushButton(tr("Rechercher"), this);
	_start->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	auto *validator = new QRegularExpressionValidator(QRegularExpression(R"([^\/:*?"<>|]*)"));
	_search->setValidator(validator);
	// if enter is pressed, start search
	_start->setDefault(true);
	connect(_search, &QLineEdit::returnPressed, _start, &QPushButton::click);
	_stop = new QPushButton(tr("Arrêter"), this);
	_stop->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	_stop->setEnabled(false);

	_layout = new QVBoxLayout(this);
	_layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	_layout->setSpacing(10);
	_layout->setContentsMargins(5, 5, 5, 5);
	auto top_layout = new QHBoxLayout;
	auto left_layout = new QVBoxLayout;

	_search_view = new SearchTableView(this);

	auto *formLayout = new QFormLayout;
	formLayout->setLabelAlignment(Qt::AlignCenter);
	formLayout->setAlignment(Qt::AlignCenter);
	formLayout->setContentsMargins(0, 0, 0, 0);
	formLayout->addRow(tr("Chercher dans : "), _remotes_comboBox);
	formLayout->addRow(tr("Nom du Fichier : "), _search);

	auto *buttonLayout = new QHBoxLayout;
	buttonLayout->setAlignment(Qt::AlignCenter);
	buttonLayout->addWidget(_start);
	buttonLayout->addWidget(_stop);

	left_layout->addLayout(formLayout);
	left_layout->setAlignment(Qt::AlignCenter);
	left_layout->addLayout(buttonLayout);
	left_layout->addWidget(_progressBar);

	top_layout->addLayout(left_layout);
	top_layout->addWidget(_filter_search);
	_layout->addLayout(top_layout);
	_layout->addWidget(_search_view);

	connectSignals();
}

void SearchWidget::connectSignals()
{
	connect(_filter_search, &FilterGroupBox::clicked, this, [this](const bool &checked)
	{
		_search->setEnabled(not checked);
	});

	connect(_stop, &QPushButton::clicked, this, [this]
	{
		_search_view->stopAllSearch();
		_progressBar->reset();
		_stop->setEnabled(false);
		_start->setEnabled(true);
	});

	connect(_start, &QPushButton::clicked, this, [this]()
	{
		auto model = dynamic_cast<QStandardItemModel *>(_remotes_comboBox->model());
		_search_view->stopAllSearch();
		_search_view->model()->removeRows(0, _search_view->model()->rowCount());
		for (int i = 0; i < model->rowCount(); i++)
		{
			auto item = model->item(i);
			if (item->checkState() == Qt::Checked)
			{
				if (_filter_search->isChecked() and not _remotes[i]->isLocal())
					_search_view->searchDistant(_filter_search->getFilters(), _remotes[i]);
				else
				{
					if (_remotes[i]->isLocal())
						_search_view->searchLocal(_search->text(), _remotes[i]);
					else
					{
						_search_view->searchDistant(
							filter::filter_file::uptr("+ *" + _search->text().toStdString() + "*", "- *"), _remotes[i]);
					}
				}
			}
		}
	});

	connect(_search_view, &SearchTableView::searchStarted, this, [this]()
	{
		_progressBar->infinite();
		_stop->setEnabled(true);
		_start->setEnabled(false);
	});

	connect(_search_view, &SearchTableView::searchFinished, this, [this]()
	{
		_progressBar->reset();
		_stop->setEnabled(false);
		_start->setEnabled(true);
	});

}

bool SearchWidget::event(QEvent *event)
{
	if (event->type() == QEvent::InputMethodQuery) { _remotes_comboBox->findChildren<QWidget *>().first()->hide(); }
	return QWidget::event(event);
}

void SearchWidget::fillRemotesComboBox()
{
	auto model = dynamic_cast<QStandardItemModel *>(_remotes_comboBox->model());
	model->clear();
	auto it = _remotes.begin();
	auto *item = new QStandardItem(it->get()->name().c_str());
	item->setIcon(QIcon(it->get()->icon().c_str()));
	item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	item->setData(Qt::Checked, Qt::CheckStateRole);
	model->appendRow(item);
	it++;
	for (; it not_eq _remotes.end(); it++)
	{
		auto *item = new QStandardItem(it->get()->name().c_str());
		item->setToolTip(it->get()->name().c_str());
		item->setIcon(QIcon(it->get()->icon().c_str()));
		item->setData(Qt::Unchecked, Qt::CheckStateRole);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(_remotes_status[*it] ? Qt::Checked : Qt::Unchecked);
		model->appendRow(item);
	}
}

void SearchWidget::showEvent(QShowEvent *event)
{
	if (_remotes not_eq Iridium::Global::remotes)
	{
		_remotes = Iridium::Global::remotes;
		fillRemotesComboBox();
	}
	QWidget::showEvent(event);
}

void SearchWidget::hideEvent(QHideEvent *event)
{
	_remotes.clear();
	auto model = dynamic_cast<QStandardItemModel *>(_remotes_comboBox->model());
	for (int i = 0; i < model->rowCount(); i++)
	{
		auto item = model->item(i);
		if (item->checkState() == Qt::Checked) { _remotes_status[_remotes[i]] = true; }
	}
	QWidget::hideEvent(event);
}
