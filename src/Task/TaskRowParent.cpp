//
// Created by sr-71 on 31/03/2024.
//

#include "TaskRowParent.hpp"

#include "Utility/Utility.hpp"

using namespace boost::variant2;

TaskRowParent::TaskRowParent(const RcloneFile& src, const RcloneFile& dest, const ire::json_log& data,
                             taskType taskType)
{
	_src = RcloneFileUPtr(new RcloneFile(src));
	_dest = RcloneFileUPtr(new RcloneFile(dest));
	_data = data;
	_taskType = taskType;
	TaskRowParent::init();
	TaskRow::connectTimer();
}

TaskRowParent::TaskRowParent(const RcloneFile& file, const ire::json_log& data, taskType taskType)
{
	_src = RcloneFileUPtr(new RcloneFile(file));
	_data = data;
	_taskType = taskType;
	TaskRowParent::init();
	TaskRow::connectTimer();
}

void TaskRowParent::finished()
{
	if (_data.get_stats() == nullptr) return;

	_size = _data.get_stats()->total_bytes;

	at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());

	setSpeed();

	terminate();
}

void TaskRowParent::init()
{
	TaskRow::init();
	at(0)->setText(_src->absolute_path().c_str());
	at(0)->setToolTip(_src->getName());

	if (_dest not_eq nullptr)
	{
		at(1)->setText(_dest->absolute_path().c_str());
		at(1)->setToolTip(_dest->getName());
	}


	switch (_taskType)
	{
		case Copy:
			at(3)->setText(QObject::tr("Copie"));
			break;
		case Move:
			at(3)->setText(QObject::tr("Déplacement"));
			break;
		case Mkdir:
			at(3)->setText(QObject::tr("Création de dossier"));
			break;
		case Delete:
			at(3)->setText(QObject::tr("Suppression"));
			break;
		case Rename:
			at(3)->setText(QObject::tr("Renommage"));
			break;
		case Unknown:
		default:
			at(3)->setText(QObject::tr("Inconnu"));
			break;
	}
}

/**
 * @brief set speed text on the item
 */
void TaskRowParent::setSpeed()
{
	if (_data.get_stats() == nullptr) return;
	at(8)->setText((Iridium::Utility::sizeToString(_data.get_stats()->speed) + "/s").c_str());
}

void TaskRowParent::updateData(const variant<ire::json_log, ire::json_log::stats::transfer>& variant)
{
	try
	{
		_data = boost::variant2::get<ire::json_log>(variant);
		updateData();
	}
	catch (const bad_variant_access& e) { throw std::runtime_error("TaskRowParent::updateData: bad_variant_access"); }
}

void TaskRowParent::updateData()
{
	if (_data.get_stats() == nullptr) return;

	_size = _data.get_stats()->total_bytes;
	at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());
	if (_size not_eq 0)
		_progressBar->setProgress(_data.get_stats()->bytes / static_cast<double_t>(_size));
	setSpeed();
	auto averageSpeedValue = _data.get_stats()->speed;

	at(9)->setText(Iridium::Utility::sizeToString(averageSpeedValue).c_str());

	auto tmp = _data.get_stats()->total_bytes;
	if (tmp > 0)
	{
		_size = tmp;
		at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());
	}
	auto speed = _data.get_stats()->speed;
	_avg.push_back(speed);
	at(8)->setText((Iridium::Utility::sizeToString(speed) + "/s").c_str());

	if (_avg.empty())
		return;

	auto avg = std::accumulate(_avg.begin(), _avg.end(), 0.0) / _avg.size();
	at(9)->setText((Iridium::Utility::sizeToString(avg) + "/s").c_str());


	// time remaining with avg speed
	auto remainingTime = static_cast<double_t>(_size - _data.get_stats()->bytes) / avg;

	auto seconds = (uint64_t)remainingTime % 60;
	auto minutes = ((uint64_t)remainingTime / 60) % 60;
	auto hours = ((uint64_t)remainingTime / 3600) % 24;
	at(6)->setText(
		QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0')));
}
