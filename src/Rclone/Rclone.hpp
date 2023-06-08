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
#include <boost/intrusive/list.hpp>
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
#include <Global.hpp>

class RcloneManager;

class Rclone : public QObject
{
Q_OBJECT

    friend class RcloneManager;

private:

    Rclone() = default;

public:
    Rclone(const Rclone &) = delete;

    Rclone &operator=(const Rclone &) = delete;

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

    enum FilterType
    {
        Include, Exclude
    };

    struct Filter
    {
        FilterType filter;
        std::string value;

        [[nodiscard]] std::string str() const
        {
            std::string str = "--filter=";
            filter == Include ? str += "+ " : str += "- ";
            return {str + value};
        }
    };

private:

    boost::process::child *_child{};

    std::unique_ptr<boost::thread> _thread{};
    std::vector<std::string> _args{}, _out{};
    std::map<std::string, std::string> _map_data{};
    int _exit{};
    Rclone::State _state{Rclone::NotLaunched};
    std::mutex _mutex{};
    std::condition_variable _cv{};

    boost::asio::io_context *_ioc{};
    boost::process::async_pipe *_pipe{};
    boost::asio::streambuf _buffer{};

    static std::map<Flag, flags_str> _map_flags;
    bool _lockable, _cancel, _resetable, _aquired = false;


public:
    void kill();

    void cancel()
    {
        _readyRead.disconnect_all_slots();
        _finished.disconnect_all_slots();
        disconnect();
        _cancel = true;
    }

    void setLockable(bool lock = true) { _lockable = lock; }

    [[nodiscard]] bool lockable() const { return _lockable; }

    void aquire() { _aquired = true; }

    [[nodiscard]] bool isAquired() const { return _aquired; }

    [[nodiscard]] bool isCanceled() const { return _cancel; }

    static void setPathRclone(const std::string &pathRclone) { Iridium::Global::path_rclone = pathRclone; }

    static std::string pathRclone() { return Iridium::Global::path_rclone; }

    void config(RemoteType type, const std::string &name, const std::vector<std::string> &args = {});

    void lsJson(const std::string &path);

    void copyTo(const RcloneFile &src, const RcloneFile &dest);

    void deleteRemote(const std::string &remote);

    void deleteFile(const RcloneFile &file);

    void mkdir(const RcloneFile &dir);

    void moveto(const RcloneFile &src, const RcloneFile &dest);

    std::vector<RemoteInfoPtr> listRemotes();

    void size(const std::string &path);

    std::string version();

    void waitForFinished();

    [[nodiscard]] State state() const { return _state; }

    [[nodiscard]] bool isRunning() const { return _state == Running; }

    void waitForStarted();

    [[nodiscard]] std::map<std::string, std::string> getData() const { return _map_data; }

    [[nodiscard]] uint8_t exitCode() const { return _exit; }

    [[nodiscard]] std::vector<std::string> readAll() { return _out; }

    static Rclone::flags_str getFlag(const Flag &key) { return _map_flags[key]; }

    static void setFlag(const Flag &key, const std::string &value) { _map_flags[key].value = value; }

    void about(const RemoteInfo &info);

    void search(const std::vector<Filter> &filters, const RemoteInfo &info);

    void clear()
    {
        _out.clear();
        _map_data.clear();
    }

    static boost::signals2::signal<void(bool)> check_rclone;

    static std::shared_ptr<Rclone> create_shared()
    {
        return std::shared_ptr<Rclone>(new Rclone());
    }

    static std::unique_ptr<Rclone> create_unique()
    {
        return std::unique_ptr<Rclone>(new Rclone());
    }

private:
    boost::signals2::signal<void(const std::string &)> _readyRead{};

    void execute();

    boost::signals2::signal<void(const int exit)> _finished{};

    static boost::json::object parseJson(const std::string &str);

    void reset();

    void readLoop(boost::process::async_pipe &pipe, std::vector<std::string> &vec);


signals:

    void started();

    void finished(int exit);

    void killed();

    void lsJsonFinished(const boost::json::array);

    void taskProgress(const boost::json::object);

    void sizeFinished(uint32_t objs, uint64_t size, std::string strSize);

    void searchRefresh(boost::json::object obj);

};

typedef std::shared_ptr<Rclone> RclonePtr;
typedef std::unique_ptr<Rclone> RcloneUniquePtr;


class RcloneManager
{

    friend class Rclone;

private:
    static std::atomic_int_fast8_t _rclone_locked;
    static std::mutex _launch_mutex;
    static std::condition_variable _launch_cv;
    static boost::thread _launch_thread;
    static std::vector<RclonePtr> _launch_queue;

public:

    static uint16_t maxProcess() { return Iridium::Global::max_process; }

    static void setMaxProcess(uint16_t nbMaxProcess) { Iridium::Global::max_process = nbMaxProcess; }

    static void addLockable(const RclonePtr &);

    static void stopThread()
    {
        _launch_thread.interrupt();
        _launch_cv.notify_one();
    }

    static void stopAll()
    {
        for (const auto &rclone: _launch_queue)
            rclone->kill();
        RcloneManager::stopThread();
    }

private:

    static void finished(Rclone *);

};

typedef std::shared_ptr<RcloneManager> RcloneManagerPtr;

#endif //IRIDIUM_RCLONE_HPP
