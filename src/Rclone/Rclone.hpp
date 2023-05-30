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

    explicit Rclone(bool lockable) { _lockable = lockable; }

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
    std::vector<std::string> _args{}, _out{}, _err{};
    std::map<std::string, std::string> _map_data{};
    uint8_t _exit{};
    Rclone::State _state{Rclone::NotLaunched};
    std::function<void(boost::process::async_pipe &pipe, std::vector<std::string> &vec)> _read_loop;
    std::mutex _mutex{};
    std::condition_variable _cv;

    boost::process::async_pipe *_pipe_out, *_pipe_err;
    boost::asio::io_context *_ioc;
    boost::asio::streambuf *_buff;

    static std::map<Flag, flags_str> _map_flags;
    bool _lockable, _cancel = false;


public:
    void kill();

    void cancel() { _cancel = true; }

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

    void listRemotes();

    void size(const std::string &path);

    std::string version();

    void waitForFinished();

    [[nodiscard]] State state() const { return _state; }

    [[nodiscard]] bool isRunning() const { return _state == Running; }

    void waitForStarted();

    [[nodiscard]] std::map<std::string, std::string> getData() const { return _map_data; }

    [[nodiscard]] std::vector<std::string> readAllError() const { return _err; }

    [[nodiscard]] uint8_t exitCode() const { return _exit; }

    [[nodiscard]] std::vector<std::string> readAll() const
    {
        auto vec = _out;
        vec.insert(vec.end(), _err.begin(), _err.end());
        return vec;
    }

    static Rclone::flags_str getFlag(const Flag &key) { return _map_flags[key]; }

    static void setFlag(const Flag &key, const std::string &value) { _map_flags[key].value = value; }

    void about(const RemoteInfo &info);

    void search(const std::vector<Filter> &filters, const RemoteInfo &info);

    static boost::signals2::signal<void()> rclone_not_exist;


private:
    boost::signals2::signal<void(const std::string &)> _readyRead{};

    void execute();

    boost::signals2::signal<void(const int exit)> _finished{};

    static boost::json::object parseJson(const std::string &str);

    void closeAll()
    {
        _readyRead.disconnect_all_slots();
        _finished.disconnect_all_slots();
        if (_pipe_out)
            _pipe_out->close();
        if (_pipe_err)
            _pipe_err->close();
        if (_ioc)
            _ioc->stop();
        disconnect();
    }


signals:

    void started();

    void finished(int exit);

    void lsJsonFinished(const boost::json::array);

    void taskProgress(const boost::json::object);

    void sizeFinished(uint32_t objs, uint64_t size, std::string strSize);

    void searchRefresh(boost::json::object obj);

};

typedef std::shared_ptr<Rclone> RclonePtr;

class RcloneManager
{

    friend class Rclone;

private:
    static std::atomic_int_fast8_t _rclone_locked;
    static std::mutex _launch_mutex;
    static std::condition_variable _launch_cv;
    static boost::thread _launch_thread;
    static std::vector<RclonePtr> _rclones, _launch_queue;

public:
    static RclonePtr get(bool lockable = false);

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
        for (const auto &rclone: _rclones)
            rclone->kill();
        RcloneManager::stopThread();
    }

    static void release(const Rclone *rclone)
    {
        std::erase_if(_rclones, [&](const auto &ptr) { return ptr.get() == rclone; });
        std::erase_if(_launch_queue, [&](const auto &ptr) { return ptr.get() == rclone; });
    }

    static void release(const RclonePtr &rclone) { release(rclone.get()); }

    static void release(const std::vector<RclonePtr> &lst) { for (const auto &rclone: lst) release(rclone); }

private:

    static void finished(Rclone *);

};

typedef std::shared_ptr<RcloneManager> RcloneManagerPtr;

#endif //IRIDIUM_RCLONE_HPP
