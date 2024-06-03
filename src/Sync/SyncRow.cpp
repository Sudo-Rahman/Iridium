//
// Created by sr-71 on 29/05/2024.
//

#include "SyncRow.hpp"

#include <QHBoxLayout>

SyncRow::SyncRow(const RcloneFilePtr &file_src, const RcloneFilePtr &file_dst) : _src(file_src), _dst(file_dst), QList()
{
	_progressBar = new LinearProgressBar();
	_progressBar->setFixedHeight(20);
	_progressBar->setRange(0, 1.0);
	_progressBar->showPercentText(true);
	_widget = new QWidget();
	_widget->setLayout(new QHBoxLayout());
	_widget->layout()->addWidget(_progressBar);
	_widget->layout()->setAlignment(Qt::AlignCenter);
	_widget->layout()->setContentsMargins(0, 0, 0, 0);

	append(new QStandardItem(file_src->absolute_path().c_str()));
	this->at(0)->setIcon(QIcon(file_src->getRemoteInfo()->icon().c_str()));
	append(new QStandardItem());
	append(new QStandardItem(file_dst->absolute_path().c_str()));
	this->at(2)->setIcon(QIcon(file_dst->getRemoteInfo()->icon().c_str()));
}

QModelIndex SyncRow::progressBarIndex() const { return this->at(1)->index(); }
QWidget *SyncRow::progressBar() const { return _widget; }
RcloneFilePtr SyncRow::src() const { return _src; }
RcloneFilePtr SyncRow::dst() const { return _dst; }

void SyncRow::setTransferData(const ire::json_log::stats::transfer &transfer)
{
	_state = Syncing;
	_progressBar->setProgress(transfer.bytes / static_cast<double>(transfer.size));
}

void SyncRow::finish()
{
	_progressBar->setProgress(1.0);
	_state = Finished;
}

void SyncRow::error(const std::string &message)
{
	_state = Error;
	_progressBar->setToolTip(message.c_str());
	_progressBar->error();
}

SyncRow::State SyncRow::state() const { return _state; }
