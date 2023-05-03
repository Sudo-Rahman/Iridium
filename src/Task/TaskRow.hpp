//
// Created by Rahman on 12/04/2023.
//

#ifndef IRIDIUM_TASKROW_HPP
#define IRIDIUM_TASKROW_HPP

#include <QStandardItem>
#include <QList>
#include <ProgressBar.hpp>
#include <QTimer>
#include <boost/json.hpp>
#include <Rclone.hpp>

class TaskRow : public QList<QStandardItem *>
{
    ProgressBar *m_progressBar{};

    QTimer m_elapsed_time{};
    uint64_t m_elapsed_time_count{};

    QString m_src{}, m_dest{};

    uint64_t m_size{};

    boost::json::object m_data{};

    std::vector<double> m_avg{};

public:

    enum State
    {
        Normal,
        Finished,
        Error
    };

    enum Type
    {
        Parent,
        Child
    };

private:
    State m_state{};

    Type m_type{};

    void init();

    void normal();

    void finished();

    void error();

    void setSpeed();

    void updateDataParent();

    void updateDataChild();

public:

    explicit TaskRow(const QString &src, const QString &dest, const boost::json::object &,
                     const Rclone::TaskType &taskType = Rclone::Unknown, const State &state = Normal,
                     const Type &type = Child);

    [[nodiscard]] ProgressBar *progressBar() { return m_progressBar; }

    [[nodiscard]] QModelIndex progressBarIndex() const { return this->at(4)->index(); };

    void updateData(const boost::json::object &);

    [[nodiscard]] const State &state() const { return m_state; }

    void setMessageToolTip(const std::string &message)
    {
        at(5)->setToolTip(message.c_str());
    }

    void setState(const State &state);


    void terminate();
};

typedef std::shared_ptr<TaskRow> TaskRowPtr;


#endif //IRIDIUM_TASKROW_HPP
