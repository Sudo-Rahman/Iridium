//
// Created by Rahman on 09/01/2023.
//


#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP

#ifndef __kernel_entry
#define __kernel_entry
#endif

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <boost/signals2.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/json.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <memory>
#include "RcloneFile.hpp"
#include <Remote.h>

class RcloneManager;

class Rclone : public QObject
{
Q_OBJECT

    friend class RcloneManager;

private:
    bool m_lockable = false;

    Rclone() = default;

    explicit Rclone(bool lockable) { m_lockable = lockable; }

public:
    Rclone(const Rclone &) = delete;

    Rclone &operator=(const Rclone &) = delete;

public:
    ~Rclone() override;

    enum State
    {
        NotLaunched, Running, Finsished, Stopped
    };

    enum TaskType
    {
        Unknown, Copy, Move, Delete, Mkdir, Rename
    };

    struct flags_str
    {
        std::string name;
        std::string value;

        [[nodiscard]] std::vector<std::string> to_vector() const
        {
            std::vector<std::string> vec;
            if (!name.empty())
                vec.emplace_back(name);
            if (!value.empty())
                vec.emplace_back(value);
            return vec;
        }
    };

    enum Flag
    {
        Transfers, Retries, Stats, Verbose,
        LogType
    };

private:

    boost::process::child m_child{};

    static std::string m_path_rclone;
    std::shared_ptr<boost::thread> m_thread{};
    std::vector<std::string> m_out{}, m_error{};
    std::map<std::string, std::string> m_map_data{};
    uint8_t m_exit{};
    Rclone::State m_state{Rclone::NotLaunched};

    std::mutex m_mutex;
    std::condition_variable m_cv;

    static std::map<Flag, flags_str> m_map_flags;

public:

    static void setPathRclone(const std::string &pathRclone);

    void config(RemoteType type, const std::string &name, const std::vector<std::string> &args = {});

    void lsJson(const std::string &path);

    void copyTo(const RcloneFile &src, const RcloneFile &dest);

    void deleteRemote(const std::string &remote);

    void deleteFile(const RcloneFile &file);

    void mkdir(const RcloneFile &dir);

    void moveto(const RcloneFile &src, const RcloneFile &dest);

    void listRemotes();

    void size(const std::string &path);

    std::string version();

    void waitForFinished();

    void terminate();

    [[nodiscard]] State state() const;

    void waitForStarted();

    [[nodiscard]] std::map<std::string, std::string> getData() const;

    [[nodiscard]] std::vector<std::string> readAllError() const;

    [[nodiscard]] uint8_t exitCode() const;

    static Rclone::flags_str getFlag(const Flag &key)
    {
        return m_map_flags[key];
    }

    static void setFlag(const Flag &key, const std::string &value)
    {
        m_map_flags[key].value = value;
    }

    void about(const RemoteInfo &info);

private:
    boost::signals2::signal<void(const std::string &)> m_readyRead{};

    void execute(const std::vector<std::string> &args);

    boost::signals2::signal<void(const int exit)> m_finished{};

    static boost::json::object parseJson(const std::string &str);

    void connectTaskSignalFinishedJson();


signals:

    void started();

    void finished(int exit);

    void lsJsonFinished(const QJsonDocument &);

    void taskProgress(const boost::json::object &);

    void sizeFinished(const uint32_t &objs, const uint64_t &size, const std::string &strSize);

    void taskFinished(const int &exit, const boost::json::object &);

};

typedef std::shared_ptr<Rclone> RclonePtr;

struct RcloneLocked
{
    RclonePtr rclone;
    std::function<void()> func;
};

class RcloneManager
{

    friend class Rclone;

private:
    static std::atomic_int_fast8_t m_nb_rclone_locked;
    static uint8_t m_nb_max_process;
    static std::mutex m_launch_mutex, m_stop_mutex, m_mutex_start;
    static std::condition_variable m_launch_cv, m_stop_cv;
    static std::deque<RclonePtr> m_rclone_vector;
    static boost::thread m_launch_thread, m_stop_thread;
    static std::deque<RcloneLocked> m_launch_queue, m_stop_queue;

public:

    static RclonePtr get();

    static RclonePtr getLockable()
    {
        auto rclone = get();
        rclone->m_lockable = true;
        return rclone;
    }

    static void release(const RclonePtr &rclone);


    static uint16_t maxProcess() { return m_nb_max_process; }

    static void setMaxProcess(uint16_t nbMaxProcess) { m_nb_max_process = nbMaxProcess; }

    static void launch(const RcloneLocked &func);

    static void stop(const std::deque<RcloneLocked> &lst);

private:

    static void finished(const Rclone *);

};

typedef std::shared_ptr<RcloneManager> RcloneManagerPtr;

#endif //IRIDIUM_RCLONE_HPP
