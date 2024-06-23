//
// Created by sr-71 on 14/06/2023.
//

#include "SyncWidget.hpp"
#include <Global.hpp>
#include <QFormLayout>
#include <QMessageBox>
#include <Sync.hpp>
#include <iridium/rclone.hpp>

#include "CircularProgressBar.hpp"
#include "IridiumApp.hpp"

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
	// _types_sync_comboBox->addItem(tr("Bisync"), QVariant::fromValue(Bisync));
	_dst_comboBox = new QComboBox(this);
	_dst_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_dst_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	auto combo_layout = new QFormLayout;

	combo_layout->setHorizontalSpacing(30);

	combo_layout->addRow(new QLabel(tr("Source"), this), _src_comboBox);
	combo_layout->addRow(new QLabel(tr("Destination"), this), _dst_comboBox);

	combo_layout->addRow(new QLabel(tr("Type de Synchronisation"), this), _types_sync_comboBox);

	_sync_button = new QPushButton(tr("Vérifier"), this);
	_sync_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_sync_button->setDefault(true);
	_stop = new QPushButton(tr("Arrêter"), this);
	_stop->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_stop->setEnabled(false);
	_sync_progressBar = new LinearProgressBar(this);
	_sync_progressBar->setMinimumHeight(12);

	auto btn_layout = new QHBoxLayout();
	btn_layout->setAlignment(Qt::AlignCenter);
	btn_layout->addWidget(_sync_button);
	btn_layout->addWidget(_stop);

	top_left_layout->addLayout(combo_layout);

	top_left_layout->addLayout(btn_layout);
	top_left_layout->addWidget(_sync_progressBar);

	top_right_layout->addWidget(_filter_group_box = new FilterGroupBox(tr("Filtres"),this));

	_view = new SyncTableView(this);
	_layout->addWidget(_view);

	connectSignals();
}

QWidget *infoWidget(uint8_t type);

void SyncWidget::connectSignals()
{
	connect(_sync_button, &QPushButton::clicked, this, [this]
	{
		switch (_state)
		{
			case None:
				{
					auto src = _src_comboBox->currentData().value<RcloneFilePtr>();
					auto dst = _dst_comboBox->currentData().value<RcloneFilePtr>();
					if (src != dst and !(src == nullptr or dst == nullptr))
					{
						_sync_progressBar->reset();
						_view->clear();
						_view->setFiles(std::move(src), std::move(dst));
						_view->analyse(_types_sync_comboBox->currentData().value<SyncType>(),
						               _filter_group_box->getFilters());
						_types_sync_comboBox->setEnabled(false);
						_filter_group_box->setEnabled(false);
					}
				}
				break;
			case Analysed:
				_view->sync(_types_sync_comboBox->currentData().value<SyncType>(), _filter_group_box->getFilters());
				break;
			default:
				break;
		}
	});

	connect(_stop, &QPushButton::clicked, this, [this]
	{
		switch (_state)
		{
			case None:
				_types_sync_comboBox->setEnabled(true);
				_filter_group_box->setEnabled(true);
				_sync_progressBar->reset();
				_sync_button->setEnabled(true);
				_view->clear();
				break;
			case Analysing:
				_state = None;
				_stop->setEnabled(false);
				_view->stop();
				break;
			case Syncing:
				_state = Analysed;
				_view->stop();
				break;
			case Synced:
				_state = None;
				_view->clear();
				_sync_progressBar->reset();
				_sync_button->setText(tr("Vérifier"));
				_sync_button->setEnabled(true);
				_stop->setEnabled(false);
				break;
			case Analysed:
				_state = None;
				_view->clear();
				_sync_button->setEnabled(true);
				_sync_button->setText(tr("Vérifier"));
				_types_sync_comboBox->setEnabled(true);
				_filter_group_box->setEnabled(true);
				_stop->setEnabled(false);
				_sync_progressBar->reset();
				_state = None;
				break;
			default:
				break;
		}
	});

	connect(_view, &SyncTableView::analyseStarted, this, [this]
	{
		_sync_progressBar->infinite();
		_sync_button->setEnabled(false);
		_stop->setEnabled(true);
		_stop->setText(tr("Arrêter"));
		_state = Analysing;
		_info_widget = infoWidget(0);
		Iridium::Global::signal_add_info(_info_widget);
	});

	connect(_view, &SyncTableView::analyseFinished, this, [this]
	{
		_sync_progressBar->reset();
		_sync_button->setEnabled(true);
		_stop->setText(tr("Effacer"));
		_state = Analysed;
		_sync_button->setText(tr("Synchroniser"));
		Iridium::Global::signal_remove_info(_info_widget);
	});

	connect(_view, &SyncTableView::syncStarted, this, [this]
	{
		_sync_button->setEnabled(false);
		_stop->setEnabled(true);
		_stop->setText(tr("Arrêter"));
		_state = Syncing;
		_info_widget = infoWidget(1);
		Iridium::Global::signal_add_info(_info_widget);
	});

	connect(_view, &SyncTableView::syncFinished, this, [this]
	{
		_sync_progressBar->setProgress(1.0);
		_stop->setText(tr("Effacer"));
		_stop->setEnabled(true);
		_state = Synced;
		Iridium::Global::signal_remove_info(_info_widget);
	});

	connect(_view, &SyncTableView::progress, this, [this](float progress)
	{
		_sync_progressBar->setProgress(progress);
		_info_widget->findChild<CircularProgressBar *>()->setProgress(progress);
	});

	connect(_view, &SyncTableView::stopped, this, [this]
	{
		_sync_progressBar->reset();
		_sync_button->setEnabled(true);
		_stop->setEnabled(true);
		_stop->setText(tr("Effacer"));
		Iridium::Global::signal_remove_info(_info_widget);
	});

	connect(_view, &SyncTableView::errorCheck, this, [this](const QString &error)
	{
		QMessageBox::critical(this, tr("Erreur"), tr("Une erreur est survenue lors de l'analyse") + "\n" + error);
		_sync_progressBar->error();
		_state = None;
		_stop->setText(tr("Effacer"));
		_sync_button->setEnabled(true);
		Iridium::Global::signal_remove_info(_info_widget);
	});

	connect(_view, &SyncTableView::errorSync, this, [this](const QString &error)
	{
		QMessageBox::critical(this, tr("Erreur"),
		                      tr("Une erreur est survenue lors de la synchronisation") + "\n" + error);
		_sync_progressBar->error();
		_state = Analysed;
		_stop->setText(tr("Effacer"));
		_stop->setEnabled(true);
		_sync_button->setEnabled(true);
		Iridium::Global::signal_remove_info(_info_widget);
	});
}

bool SyncWidget::event(QEvent *event)
{
	if (event->type() == QEvent::ShowToParent)
	{
		auto selected_src = _src_comboBox->currentData().value<RcloneFilePtr>();
		auto selected_dst = _dst_comboBox->currentData().value<RcloneFilePtr>();
		_src_comboBox->clear();
		_dst_comboBox->clear();
		for (auto &dir: Iridium::Global::sync_dirs)
		{
			_src_comboBox->addItem(dir->parent() != nullptr ? dir->getName() : QString(dir->getRemoteInfo()->full_path().c_str()));
			_src_comboBox->setItemData(_src_comboBox->count() - 1, QVariant::fromValue(dir), Qt::UserRole);
			_src_comboBox->setItemIcon(_src_comboBox->count() - 1, dir->getRemoteInfo()->getIcon());
			_dst_comboBox->addItem(dir->parent() != nullptr ? dir->getName() : QString(dir->getRemoteInfo()->full_path().c_str()));
			_dst_comboBox->setItemData(_dst_comboBox->count() - 1, QVariant::fromValue(dir), Qt::UserRole);
			_dst_comboBox->setItemIcon(_dst_comboBox->count() - 1, dir->getRemoteInfo()->getIcon());
		}
		if (selected_src)
		{
			auto index = _src_comboBox->findData(QVariant::fromValue(selected_src));
			if (index != -1)
				_src_comboBox->setCurrentIndex(index);
		}
		if (selected_dst)
		{
			auto index = _dst_comboBox->findData(QVariant::fromValue(selected_dst));
			if (index != -1)
				_dst_comboBox->setCurrentIndex(index);
		}
	}
	return QWidget::event(event);
}

QWidget *infoWidget(uint8_t type)
{
	auto widget = new QWidget();
	auto layout = new QHBoxLayout(widget);
	auto progressBar = new CircularProgressBar(widget);
	progressBar->setSize(20);
	layout->addWidget(progressBar);
	layout->addWidget(new QLabel("", widget));
	switch (type)
	{
		case 0:
			widget->findChild<QLabel *>()->setText(QObject::tr("Analyse en cours"));
			break;
		case 1:
			widget->findChild<QLabel *>()->setText(QObject::tr("Synchronisation en cours"));
			break;
		default:
			break;
	}
	return widget;
}
