//
// Created by sr-71 on 14/06/2023.
//

#include "SyncWidget.hpp"
#include <Global.hpp>
#include <QEvent>
#include <QFormLayout>
#include <QMessageBox>
#include <Sync.hpp>

SyncWidget::SyncWidget(QWidget *parent) : QWidget(parent)
{
	_layout = new QVBoxLayout(this);
	_layout->setContentsMargins(5, 5, 5, 5);
	_layout->setSpacing(10);
	auto top_layout = new QHBoxLayout();
	auto top_right_layout = new QVBoxLayout();
	auto top_left_layout = new QVBoxLayout();

	top_layout->addLayout(top_left_layout, 1);
	top_layout->addLayout(top_right_layout, 1);
	_layout->addLayout(top_layout);

	_src_comboBox = new QComboBox(this);
	_src_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_src_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	_types_sync_comboBox = new QComboBox(this);
	_types_sync_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_types_sync_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	_types_sync_comboBox->addItem(tr("Sync"), QVariant::fromValue(Sync));
	_types_sync_comboBox->addItem(tr("Bisync"), QVariant::fromValue(Bisync));
	_dst_comboBox = new QComboBox(this);
	_dst_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_dst_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	auto combo_layout = new QFormLayout;

	combo_layout->setHorizontalSpacing(30);

	combo_layout->addRow(new QLabel(tr("Source"), this), _src_comboBox);
	combo_layout->addRow(new QLabel(tr("Destination"), this), _dst_comboBox);

	combo_layout->addRow(new QLabel(tr("Type de Synchronisation"), this), _types_sync_comboBox);

	_sync_button = new QPushButton(tr("Verification"), this);
	_sync_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_sync_button->setDefault(true);
	_stop = new QPushButton(tr("Annuler"), this);
	_stop->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_stop->hide();
	_sync_progressBar = new LinearProgressBar(this);
	_sync_progressBar->setMinimumHeight(15);
	_sync_progressBar->hide();

	top_left_layout->addLayout(combo_layout);

	top_left_layout->addWidget(_sync_button, 0, Qt::AlignCenter);
	top_left_layout->addWidget(_stop, 0, Qt::AlignCenter);
	top_left_layout->addWidget(_sync_progressBar);

	top_right_layout->addWidget(_filter_group_box = new FilterGroupBox(this));

	_view = new SyncTableView(this);
	_layout->addWidget(_view);

	connectSignals();
}

void SyncWidget::connectSignals()
{
	connect(_sync_button, &QPushButton::clicked, this, [this]
	{
		switch (_state)
		{
			case None:
				_state = Analysing;
				_sync_progressBar->setState(LinearProgressBar::Progress);
				_view->clear();
				_view->setFiles(_src_comboBox->currentData().value<RcloneFilePtr>(),
				                _dst_comboBox->currentData().value<RcloneFilePtr>());
				_view->analyse(_src_comboBox->currentData().value<SyncType>(), _filter_group_box->getFilters());
				break;
			case Analysed:
				_view->sync(_src_comboBox->currentData().value<SyncType>(), _filter_group_box->getFilters());
				_state = Syncing;
				break;
			case Synced:
				_view->clear();
				_sync_progressBar->hide();
				_sync_button->setText(tr("Verification"));
				_state = None;
			default: break;
		}
	});

	connect(_stop, &QPushButton::clicked, this, [this]
	{
		_stop->hide();
		_sync_button->show();
		_view->stop();
		if (_state == Analysing)
		{
			_state = None;
			_view->reset();
		}
		else if (_state == Syncing) { _state = Analysed; }
	});

	connect(_view, &SyncTableView::analyseStarted, this, [this]
	{
		_sync_progressBar->show();
		_sync_progressBar->setRange(0, 0);
		_sync_button->hide();
		_stop->show();
		_state = Analysing;
	});

	connect(_view, &SyncTableView::analyseFinished, this, [this]
	{
		_sync_progressBar->setRange(0, 1.0);
		_sync_progressBar->setProgress(0);
		_sync_button->show();
		_stop->hide();
		_state = Analysed;
		_sync_button->setText(tr("Synchroniser"));
	});

	connect(_view, &SyncTableView::syncStarted, this, [this]
	{
		_sync_button->hide();
		_stop->show();
		_state = Syncing;
	});

	connect(_view, &SyncTableView::syncFinished, this, [this]
	{
		_sync_progressBar->setProgress(1.0);
		_sync_button->show();
		_stop->hide();
		_state = Synced;
		_sync_button->setText(tr("effacer"));
	});

	connect(_view, &SyncTableView::progress, this, [this](float progress)
	{
		_sync_progressBar->setProgress(progress);
	});

	connect(_view, &SyncTableView::errorCheck, this, [this]
	{
		QMessageBox::critical(this, tr("Erreur"), tr("Une erreur est survenue lors de l'analyse"));
		_sync_progressBar->hide();
		_sync_button->show();
		_stop->hide();
		_state = None;
	});

	connect(_view, &SyncTableView::errorSync, this, [this]
	{
		QMessageBox::critical(this, tr("Erreur"), tr("Une erreur est survenue lors de la synchronisation"));
		_sync_progressBar->hide();
		_sync_button->show();
		_stop->hide();
		_state = Analysed;
	});
}

bool SyncWidget::event(QEvent *event)
{
	if (event->type() == QEvent::ShowToParent)
	{
		_src_comboBox->clear();
		_dst_comboBox->clear();
		for (auto &dir: Iridium::Global::sync_dirs)
		{
			_src_comboBox->addItem(dir->getName());
			_src_comboBox->setItemData(_src_comboBox->count() - 1, QVariant::fromValue(dir), Qt::UserRole);
			_src_comboBox->setItemIcon(_src_comboBox->count() - 1, dir->getIcon());
			_dst_comboBox->addItem(dir->getName());
			_dst_comboBox->setItemData(_dst_comboBox->count() - 1, QVariant::fromValue(dir), Qt::UserRole);
			_dst_comboBox->setItemIcon(_dst_comboBox->count() - 1, dir->getIcon());
		}
	}
	return QWidget::event(event);
}
