//
// Created by sr-71 on 12/04/2023.
//

#include "TaskRow.hpp"
#include <Utility/Utility.hpp>


/**
 * @brief TaskRow::TaskRow
 * @param src
 * @param dest
 * @param data
 * @param id
 */
TaskRow::TaskRow(const QString &src, const QString &dest, const boost::json::object &data,const size_t &id)
	: QList<QStandardItem *>()
{

	m_id = id;
	m_src = src;
	m_dest = dest;

	m_progressBar = new QProgressBar;
	m_progressBar->setRange(0, 100);
	m_progressBar->setValue(
		data.at("percentage").as_int64()
	);

	m_elapsedTime.setInterval(1000);
	m_elapsedTime.start();

	auto *source = new QStandardItem;
	source->setText(src);
	source->setToolTip(src);

	auto *destination = new QStandardItem;
	destination->setText(dest);
	destination->setToolTip(dest);

	auto *size = new QStandardItem;
	try
	{ m_size = data.at("size").as_int64(); }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{ m_size = 0; }

	size->setText(Iridium::Utility::sizeToString(m_size).c_str());

	auto *progress = new QStandardItem;

	auto *status = new QStandardItem;
	status->setText(QObject::tr("En cours"));

	auto *remainingTime = new QStandardItem;
	remainingTime->setText("00:00:00");

	auto *elapsedTime = new QStandardItem;
	elapsedTime->setText("00:00:00");

	auto *speed = new QStandardItem;
	double64_t speedValue;
	try
	{
		speedValue = data.at("speed").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{
		speedValue = 0;
	}
	speed->setText(Iridium::Utility::sizeToString(speedValue).c_str());

	auto *averageSpeed = new QStandardItem;

	double64_t averageSpeedValue;
	try
	{
		averageSpeedValue = data.at("speedAvg").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{
		averageSpeedValue = 0;
	}
	averageSpeed->setText(Iridium::Utility::sizeToString(averageSpeedValue).c_str());


	append(source);
	append(destination);
	append(size);
	append(progress);
	append(status);
	append(remainingTime);
	append(elapsedTime);
	append(speed);
	append(averageSpeed);

	QObject::connect(&m_elapsedTime, &QTimer::timeout, [this, elapsedTime]()
	{
		m_elapsedTimeCount++;
		// m_elapsedTimeCount to hh:mm:ss
		auto seconds = m_elapsedTimeCount % 60;
		auto minutes = (m_elapsedTimeCount / 60) % 60;
		auto hours = (m_elapsedTimeCount / 3600) % 24;
		elapsedTime->setText(
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
	m_progressBar->setValue(
		data.at("percentage").as_int64()
	);


	double64_t speed;
	double64_t speedAvg;

	try
	{
		speed = data.at("speed").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{ speed = 0; }

	try
	{
		speedAvg = data.at("speedAvg").as_double();
	} catch (boost::wrapexcept<std::invalid_argument> &e)
	{ speedAvg = 0; }

	double64_t remainingTime;
	try
	{ remainingTime = double64_t (m_size - data.at("bytes").as_int64()) / speed; }
	catch (boost::wrapexcept<std::invalid_argument> &e)
	{ remainingTime = 0; }

	// remainingTime to hh:mm:ss
	auto seconds = (uint64_t) remainingTime % 60;
	auto minutes = ((uint64_t) remainingTime / 60) % 60;
	auto hours = ((uint64_t) remainingTime / 3600) % 24;

	this->at(5)->setText(
		QString("%1:%2:%3")
			.arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0')));

	this->at(7)->setText((Iridium::Utility::sizeToString(speed) + "/s").c_str());
	this->at(8)->setText((Iridium::Utility::sizeToString(speedAvg) + "/s").c_str());
}

/**
 * @brief terminate the task
 */
void TaskRow::terminate()
{
	m_elapsedTime.stop();
	m_progressBar->setValue(100);
	this->at(4)->setText(QObject::tr("Terminé"));
}
