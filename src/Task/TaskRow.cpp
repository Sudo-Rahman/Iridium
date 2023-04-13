//
// Created by sr-71 on 12/04/2023.
//

#include "TaskRow.hpp"
#include <Utility/Utility.hpp>


/**
 * @brief TaskRow::TaskRow
 * @param src
 * @param dest
 * @param m_data
 * @param id
 * @param state
 */
TaskRow::TaskRow(const QString &src, const QString &dest, const boost::json::object &data, const size_t &id,
				 const Rclone::TaskType &taskType, const State &state,const Type &type)
	: QList<QStandardItem *>()
{

	m_state = state;

	switch (state)
	{
		case Normal:
		case Error:
			m_data = data;
			break;
		case Finished:
			m_data = data.at("stats").as_object();
			break;
	}

	m_id = id;
	m_src = src;
	m_dest = dest;

	init();

	switch (state)
	{
		case Normal:
			normal();
			break;
		case Finished:
			finished();
			break;
		case Error:
			error();
			break;
	}

	switch (taskType)
	{
		case Rclone::Copy:
			at(3)->setText(QObject::tr("Copie"));
			break;
		case Rclone::Move:
			at(3)->setText(QObject::tr("Déplacement"));
			break;
		case Rclone::Mkdir:
			at(3)->setText(QObject::tr("Création de dossier"));
			break;
		case Rclone::Delete:
			at(3)->setText(QObject::tr("Suppression"));
			break;
		case Rclone::Rename:
			at(3)->setText(QObject::tr("Renommage"));
			break;
		case Rclone::Unknown:
		default:
			at(3)->setText(QObject::tr("Inconnu"));
			break;
	}

	QObject::connect(&m_elapsedTime, &QTimer::timeout, [this]()
	{
		m_elapsedTimeCount++;
		// m_elapsedTimeCount to hh:mm:ss
		auto seconds = m_elapsedTimeCount % 60;
		auto minutes = (m_elapsedTimeCount / 60) % 60;
		auto hours = (m_elapsedTimeCount / 3600) % 24;
		at(7)->setText(
			QString("%1:%2:%3")
				.arg(hours, 2, 10, QChar('0'))
				.arg(minutes, 2, 10, QChar('0'))
				.arg(seconds, 2, 10, QChar('0')));
	});
}

/**
 * @brief update data of the items
 * @param data
 */
void TaskRow::updateData(const boost::json::object &data)
{
	if (m_state == TaskRow::Finished)
		return;

	try
	{
		m_progressBar->setValue(
			data.at("percentage").as_int64());
	}
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{ return; }
	catch (boost::wrapexcept<std::out_of_range> &e)
	{ return; }

	double64_t speed = 0;
	double64_t speedAvg = 0;

	try
	{
		speed = data.at("speed").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}


	try
	{
		speedAvg = data.at("speedAvg").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}

	double64_t remainingTime = 0;
	try
	{ remainingTime = double64_t(m_size - data.at("bytes").as_int64()) / speed; }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}


	// remainingTime to hh:mm:ss
	auto seconds = (uint64_t) remainingTime % 60;
	auto minutes = ((uint64_t) remainingTime / 60) % 60;
	auto hours = ((uint64_t) remainingTime / 3600) % 24;

	this->at(6)->setText(
		QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0')));

	this->at(8)->setText((Iridium::Utility::sizeToString(speed) + "/s").c_str());
	this->at(9)->setText((Iridium::Utility::sizeToString(speedAvg) + "/s").c_str());
}

/**
 * @brief terminate the task
 */
void TaskRow::terminate()
{
	m_elapsedTime.stop();
	m_progressBar->setValue(100);
	this->at(5)->setText(QObject::tr("Terminé"));
	this->at(6)->setText(QObject::tr("00:00:00"));
	m_state = TaskRow::Finished;
}

/**
 * @brief set speed text on the item
 */
void TaskRow::setSpeed()
{
	double64_t speedValue = 0;
	try
	{
		speedValue = m_data.at("speed").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}

	at(8)->setText((Iridium::Utility::sizeToString(speedValue) + "/s").c_str());
}


/**
 * @brief task normal launch and running
 */
void TaskRow::normal()
{
	try
	{ m_size = m_data.at("size").as_int64(); }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}
	at(2)->setText(Iridium::Utility::sizeToString(m_size).c_str());

	try
	{ m_progressBar->setValue(m_data.at("percentage").as_int64()); }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{ m_progressBar->setValue(0); }
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}

	setSpeed();

	double64_t averageSpeedValue = 0;
	try
	{ averageSpeedValue = m_data.at("speedAvg").as_double(); }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}
	at(9)->setText(Iridium::Utility::sizeToString(averageSpeedValue).c_str());
}

/**
 * @brief if the task is finished
 */
void TaskRow::finished()
{
	try
	{ m_size = m_data.at("totalBytes").as_int64(); }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{}
	catch (boost::wrapexcept<std::out_of_range> &e)
	{}

	at(2)->setText(Iridium::Utility::sizeToString(m_size).c_str());

	m_progressBar->setValue(100);

	setSpeed();

	terminate();
}

/**
 * @brief if an error occur
 */
void TaskRow::error()
{
	m_elapsedTime.stop();
	m_progressBar->setValue(100);
	this->at(5)->setText(QObject::tr("Erreur"));
	this->at(5)->setToolTip(m_data.at("error").as_string().c_str());
	this->at(6)->setText("00:00:00");
}

/**
 * @brief init all items
 */
void TaskRow::init()
{
	m_progressBar = new QProgressBar;
	m_progressBar->setRange(0, 100);
	m_progressBar->setValue(0);
	m_progressBar->setTextVisible(true);

	m_elapsedTime.setInterval(1000);
	m_elapsedTime.start();

	auto *source = new QStandardItem;

	auto *destination = new QStandardItem;

	auto *size = new QStandardItem;

	auto *type = new QStandardItem;

	auto *progress = new QStandardItem;

	auto *status = new QStandardItem;

	auto *remainingTime = new QStandardItem;

	auto *elapsedTime = new QStandardItem;

	auto *speed = new QStandardItem;

	auto *averageSpeed = new QStandardItem;

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

	source->setText(m_src);
	source->setToolTip(m_src);

	destination->setText(m_dest);
	destination->setToolTip(m_dest);

	status->setText(QObject::tr("En cours"));

	remainingTime->setText("00:00:00");

	elapsedTime->setText("00:00:00");

	speed->setText("0 B/s");
	averageSpeed->setText("0 B/s");
}