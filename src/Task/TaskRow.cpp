//
// Created by Rahman on 12/04/2023.
//

#include "TaskRow.hpp"
#include <Utility/Utility.hpp>


/**
 * @brief TaskRow::TaskRow
 * @param src
 * @param dest
 * @param _data
 * @param id
 * @param state
 */
TaskRow::TaskRow(const QString &src, const QString &dest, const boost::json::object &data,
                 const Rclone::TaskType &taskType, const State &state, const Type &type)
        : QList<QStandardItem *>()
{

    _type = type;

    switch (state)
    {
        case Normal:
        case Error:
            _data = data;
            break;
        case Finished:
            _data = data.at("stats").as_object();
            break;
        case Cancelled:
            break;
    }

    _src = src;
    _dest = dest;

    init();

    if (_type == Child)
        setState(state);

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
 * @brief update data of the items
 * @param data
 */
void TaskRow::updateData(const boost::json::object &data)
{
    if (_state == TaskRow::Finished)
        return;

    _data = data;

    if (Type::Parent == _type)
        updateDataParent();
    else
        updateDataChild();
}

void TaskRow::updateDataParent()
{
    try
    {
        auto tmp = _data.at("totalBytes").as_int64();
        if (tmp != _size)
        {
            _size = tmp;
            at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());
        }
        auto bytes = _data.at("bytes").as_int64();

        if (_size not_eq 0)
            _progressBar->setValue((int) (bytes * 100 / _size));
        auto speed = _data.at("speed").as_double();
        _avg.push_back(speed);
        at(8)->setText((Iridium::Utility::sizeToString(speed) + "/s").c_str());

        if (_avg.empty())
            return;

        auto avg = std::accumulate(_avg.begin(), _avg.end(), 0.0) / _avg.size();
        at(9)->setText((Iridium::Utility::sizeToString(avg) + "/s").c_str());


        // time remaining with avg speed
        auto remainingTime = double64_t(_size - _data.at("bytes").as_int64()) / avg;

        auto seconds = (uint64_t) remainingTime % 60;
        auto minutes = ((uint64_t) remainingTime / 60) % 60;
        auto hours = ((uint64_t) remainingTime / 3600) % 24;
        at(6)->setText(
                QString("%1:%2:%3")
                        .arg(hours, 2, 10, QChar('0'))
                        .arg(minutes, 2, 10, QChar('0'))
                        .arg(seconds, 2, 10, QChar('0')));
    }    catch (boost::exception &e) {}

}

void TaskRow::updateDataChild()
{
    try
    {
        _progressBar->setValue(
                _data.at("percentage").as_int64());

    }catch (boost::exception &e) {}


    double64_t speed = 0;
    double64_t speedAvg = 0;
    double64_t remainingTime = 0;

    try
    {
        speed = _data.at("speed").as_double();
        if (speed == 0)
            return;
        speedAvg = _data.at("speedAvg").as_double();
        remainingTime = double64_t(_size - _data.at("bytes").as_int64()) / speed;
    }
    catch (boost::exception &e) {}


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
    _elapsed_time.stop();
    _progressBar->setValue(100);
    _progressBar->setToolTip("100%");
    this->at(5)->setText(QObject::tr("Terminé"));
    this->at(6)->setText(QObject::tr("00:00:00"));
    _state = TaskRow::Finished;
}

/**
 * @brief set speed text on the item
 */
void TaskRow::setSpeed()
{
    try
    {
        double64_t speedValue;
        speedValue = _data.at("speed").as_double();
        at(8)->setText((Iridium::Utility::sizeToString(speedValue) + "/s").c_str());
    } catch (boost::exception &e) {}
}


/**
 * @brief task normal launch and running
 */
void TaskRow::normal()
{
    double64_t averageSpeedValue = 0;
    try
    {
        _size = _data.at("size").as_int64();
        at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());
        _progressBar->setValue(_data.at("percentage").as_int64());
        setSpeed();
        averageSpeedValue = _data.at("speedAvg").as_double();
    }
    catch (boost::exception &e) {}
    at(9)->setText(Iridium::Utility::sizeToString(averageSpeedValue).c_str());
}

/**
 * @brief if the task is finished
 */
void TaskRow::finished()
{
    try { _size = _data.at("totalBytes").as_int64(); }
    catch (boost::exception &e) {}

    at(2)->setText(Iridium::Utility::sizeToString(_size).c_str());

    _progressBar->setValue(100);

    setSpeed();

    terminate();
}

/**
 * @brief if an error occur
 */
void TaskRow::error()
{
    _state = TaskRow::Error;
    _elapsed_time.stop();
    _progressBar->setValue(0);
    _progressBar->setToolTip("0%");
    _progressBar->error();
    this->at(5)->setText(QObject::tr("Erreur"));
    try
    {
        this->at(5)->setToolTip(_data.at("msg").as_string().c_str());

    } catch (boost::exception &e) {}
    this->at(6)->setText("00:00:00");
}

/**
 * @brief init all items
 */
void TaskRow::init()
{
    _progressBar = new ProgressBar(ProgressBar::Linear);
    _progressBar->setRange(0, 100);
    _progressBar->setValue(0);
    _progressBar->setShowProgress(true);
    _progressBar->setMaximumHeight(20);

    _elapsed_time.setInterval(1000);
    _elapsed_time.start();

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

    source->setText(_src);
    source->setToolTip(_src);

    destination->setText(_dest);
    destination->setToolTip(_dest);

    status->setText(QObject::tr("En cours"));

    remainingTime->setText("00:00:00");

    elapsedTime->setText("00:00:00");

    speed->setText("0 B/s");
    averageSpeed->setText("0 B/s");
}

void TaskRow::cancel() {
    if (_state not_eq TaskRow::Finished and _state not_eq TaskRow::Error)
    {
        _state = TaskRow::Cancelled;
        _elapsed_time.stop();
        _progressBar->error();
        this->at(5)->setText(QObject::tr("Annulé"));
        this->at(6)->setText("00:00:00");
        this->at(7)->setText("00:00:00");
    }
}

void TaskRow::setState(const TaskRow::State &state)
{
    _state = state;
    switch (state)
    {
        case TaskRow::Normal:
            normal();
            break;
        case TaskRow::Finished:
            finished();
            break;
        case TaskRow::Error:
            error();
            break;
        case TaskRow::Cancelled:
            cancel();
            break;
    }
}
