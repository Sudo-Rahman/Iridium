//
// Created by sr-71 on 31/03/2024.
//

#include "TaskRow.hpp"
LinearProgressBar* TaskRow::progressBar() const { return _progressBar; }

QModelIndex TaskRow::progressBarIndex() const
{
	if (this->size() < 4)
		throw std::runtime_error("TaskRow::progressBarIndex() : size() < 4");
	return at(4)->index();
}

TaskRow::State TaskRow::state() const { return _state; }

void TaskRow::setMessageToolTip(const std::string& message) const
{
	if (this->size() < 5)
		throw std::runtime_error("TaskRow::setMessageToolTip() : size() < 5");
	at(5)->setToolTip(message.c_str());
}

void TaskRow::cancel()
{
	if (_state not_eq Finished and _state not_eq Error)
	{
		_state = Cancelled;
		_elapsed_time.stop();
		_progressBar->error();
		this->at(5)->setText(QObject::tr("Annulé"));
		this->at(6)->setText("00:00:00");
		this->at(7)->setText("00:00:00");
	}
}

void TaskRow::terminate()
{
	_elapsed_time.stop();
	_progressBar->setProgress(1);
	_progressBar->setToolTip("100%");
	this->at(5)->setText(QObject::tr("Terminé"));
	this->at(6)->setText(QObject::tr("00:00:00"));
	_state = Finished;
}

void TaskRow::connectTimer()
{
	QObject::connect(&_elapsed_time, &QTimer::timeout, [this]()
	{
		_elapsed_time_count++;
		// _elapsed_time_count to hh:mm:ss
		auto seconds = _elapsed_time_count % 60;
		auto minutes = (_elapsed_time_count / 60) % 60;
		auto hours = (_elapsed_time_count / 3600) % 24;
		at(7)->setText(
			QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0')));
	});
}

/**
* @brief init all items
*/
void TaskRow::init()
{
	_progressBar = new LinearProgressBar();
	_progressBar->setRange(0, 1);
	_progressBar->setProgress(0);
	_progressBar->showPercentText(true);
	_progressBar->setFixedHeight(20);

	_elapsed_time.setInterval(1000);
	_elapsed_time.start();

	auto * source = new QStandardItem;

	auto * destination = new QStandardItem;

	auto * size = new QStandardItem;

	auto * type = new QStandardItem;

	auto * progress = new QStandardItem;

	auto * status = new QStandardItem;

	auto * remainingTime = new QStandardItem;

	auto * elapsedTime = new QStandardItem;

	auto * speed = new QStandardItem;

	auto * averageSpeed = new QStandardItem;

	append(source);
	append(destination);
	append(size);
	append(type);
	append(progress);
	append(status);
	append(remainingTime);
	append(elapsedTime);
	append(speed);
	append(averageSpeed);

	for (int i = 0; i < this->size() and (i not_eq 4); ++i)
		this->at(i)->setText("--");

	status->setText(QObject::tr("En cours"));

	remainingTime->setText("00:00:00");

	elapsedTime->setText("00:00:00");

	speed->setText("0 B/s");
	averageSpeed->setText("0 B/s");
}

void TaskRow::error(const std::string& message)
{
	_state = Error;
	_elapsed_time.stop();
	_progressBar->error();
	this->at(5)->setText(QObject::tr("Erreur"));
	this->at(5)->setToolTip(message.c_str());
	this->at(6)->setText("00:00:00");
}
