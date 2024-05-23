//
// Created by sr-71 on 14/06/2023.
//

#include "SyncWidget.hpp"
#include <Global.hpp>
#include <QEvent>

SyncWidget::SyncWidget(QWidget *parent) : QWidget(parent)
{

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(5, 5, 5, 5);
    _layout->setSpacing(10);
    auto top_layout = new QHBoxLayout();
    auto top_right_layout = new QVBoxLayout();
    auto top_left_layout = new QVBoxLayout();

    top_layout->addLayout(top_left_layout);
    top_layout->addLayout(top_right_layout);
    _layout->addLayout(top_layout);

    _src_comboBox = new QComboBox(this);
    _src_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _src_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _dst_comboBox = new QComboBox(this);
    _dst_comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _dst_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    auto combo_layout = new QGridLayout();
    combo_layout->setHorizontalSpacing(30);

    combo_layout->addWidget(new QLabel(tr("Source"), this), 0, 0);
    combo_layout->addWidget(_src_comboBox, 1, 0);

    combo_layout->addWidget(new QLabel(tr("Destination"), this), 0, 1);
    combo_layout->addWidget(_dst_comboBox, 1, 1);

    _sync_button = new QPushButton(tr("Verification"), this);
    _sync_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _stop = new QPushButton(tr("Annuler"), this);
    _stop->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _stop->hide();
    _sync_progressBar = new LinearProgressBar( this);
    _sync_progressBar->setMinimumHeight(15);

    top_left_layout->addLayout(combo_layout);

    top_left_layout->addWidget(_sync_button, 0, Qt::AlignCenter);
    top_left_layout->addWidget(_stop, 0, Qt::AlignCenter);
    top_left_layout->addWidget(_sync_progressBar);

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
                _state = Analyse;
                break;
            case Analyse:
                _state = Sync;
                break;
            case Sync:
                break;
        }
        _stop->show();
        _sync_button->hide();
    });

    connect(_stop, &QPushButton::clicked, this, [this]
    {
        _state = None;
        _stop->hide();
        _sync_button->show();
        _view->reset();
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