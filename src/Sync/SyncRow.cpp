//
// Created by sr-71 on 29/05/2024.
//

#include "SyncRow.hpp"

#include <QHBoxLayout>

#include "Utility/Utility.hpp"

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
	this->at(0)->setData(file_src->absolute_path().c_str(), Qt::UserRole + 1);
	append(new QStandardItem());
	append(new QStandardItem(file_dst->absolute_path().c_str()));
	this->at(2)->setIcon(QIcon(file_dst->getRemoteInfo()->icon().c_str()));
	this->at(2)->setData(file_dst->absolute_path().c_str(), Qt::UserRole + 1);

	auto *size = new QStandardItem;

	auto *remainingTime = new QStandardItem;

	auto *elapsedTime = new QStandardItem;

	auto *speed = new QStandardItem;

	auto *averageSpeed = new QStandardItem;

	append(size);
	append(remainingTime);
	append(elapsedTime);
	append(speed);
	append(averageSpeed);

	_elapsed_time.setInterval(1000);
	QObject::connect(&_elapsed_time, &QTimer::timeout, [this]()
	{
		_elapsed_time_count++;
		// _elapsed_time_count to hh:mm:ss
		auto seconds = _elapsed_time_count % 60;
		auto minutes = (_elapsed_time_count / 60) % 60;
		auto hours = (_elapsed_time_count / 3600) % 24;
		at(5)->setText(
			QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0')));
		at(5)->setData(_elapsed_time_count, Qt::UserRole + 1);
	});

	//remainingTime
	remainingTime->setText("00:00:00");

	//elapsedTime
	elapsedTime->setText("00:00:00");

	//speed
	speed->setText("--");
	//averageSpeed
	averageSpeed->setText("--");
}

QModelIndex SyncRow::progressBarIndex() const { return this->at(1)->index(); }
QWidget *SyncRow::progressBar() const { return _widget; }
RcloneFilePtr SyncRow::src() const { return _src; }
RcloneFilePtr SyncRow::dst() const { return _dst; }

void SyncRow::setTransferData(const ire::json_log::stats::transfer &transfer)
{
	if (_state == None) { _elapsed_time.start(); }
	_state = Syncing;
	_progressBar->setProgress(transfer.percentage / 100.0);
	at(1)->setData(transfer.percentage / 100.0, Qt::UserRole + 1);

	at(3)->setText(Iridium::Utility::sizeToString(transfer.size).c_str());
	at(3)->setData(transfer.size, Qt::UserRole + 1);

	auto remainingTime = static_cast<double_t>(transfer.size - transfer.bytes) / transfer.speed;
	// remainingTime to hh:mm:ss
	auto seconds = (uint64_t)remainingTime % 60;
	auto minutes = ((uint64_t)remainingTime / 60) % 60;
	auto hours = ((uint64_t)remainingTime / 3600) % 24;

	at(4)->setText(
		QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0')));
	at(4)->setData(remainingTime, Qt::UserRole + 1);

	this->at(6)->setText((Iridium::Utility::sizeToString(transfer.speed) + "/s").c_str());
	this->at(6)->setData(transfer.speed, Qt::UserRole + 1);
	this->at(7)->setText((Iridium::Utility::sizeToString(transfer.speed_avg) + "/s").c_str());
	this->at(7)->setData(transfer.speed_avg, Qt::UserRole + 1);
}

void SyncRow::finish()
{
	_progressBar->setProgress(1.0);
	at(1)->setData(1.0, Qt::UserRole + 1);
	_state = Finished;
	_elapsed_time.stop();
}

void SyncRow::error(const std::string &message)
{
	_state = Error;
	_progressBar->setToolTip(message.c_str());
	at(1)->setData(-1.0, Qt::UserRole + 1);
	_progressBar->error();
	_elapsed_time.stop();
}

SyncRow::State SyncRow::state() const { return _state; }
