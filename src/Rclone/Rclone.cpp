//
// Created by Rahman on 09/01/2023.
//

#include <Rclone.hpp>

#include <iostream>

#include <Utility/Utility.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/chrono.hpp>
#include <regex>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>


#ifdef _WIN32
#include <boost/process/windows.hpp>
#include <codecvt>
#endif

namespace bp = boost::process;
namespace bj = boost::json;
namespace ba = boost::asio;
namespace bfs = boost::filesystem;
using namespace std;

std::map<Rclone::Flag, Rclone::flags_str> Rclone::_map_flags{
        {Rclone::Flag::Transfers, {"--transfers", "4"}},
        {Rclone::Flag::Stats,     {"--stats",     "0.5s"}},
        {Rclone::Flag::Retries,   {"--retries",   "3"}},
        {Rclone::Flag::Verbose,   {"",            "-v"}},
        {Rclone::Flag::LogType,   {"",            "--use-json-log"}},
};
boost::signals2::signal<void(bool)> Rclone::check_rclone;

/**
 * @brief Rclone::lsJson, list a directory in json format
 * @param path
 */
void Rclone::lsJson(const RcloneFile &path, bool recursive)
{
    _readyRead.connect(
            [this](const string &data)
            {
                try
                {
                    regex reg(R"(\{.*\})");
                    smatch match;
                    if (regex_search(data, match, reg))
                    {
                        auto obj = bj::parse(match[0].str());
                        emit readDataJson(std::move(obj.as_object()));
                    }
                } catch (boost::exception &e)
                {
//                        emit lsJsonFinished(std::move(bj::array()));
                    cerr << boost::algorithm::join(_out, "") << endl;
                    cerr << "Error: " << boost::diagnostic_information(e) << endl;
                }
            });
    _args = {"lsjson", path.getPath().toStdString(), "--drive-skip-gdocs", "--fast-list"};
    if (recursive)
        _args.emplace_back("--recursive");
    if (not _lockable)
        execute();
}

/**
 * @brief Rclone::copyTo, copy a file to a destination
 * @param src
 * @param dest
 */
void Rclone::copyTo(const RcloneFile &src, const RcloneFile &dest)
{
    _args =
            {"copyto", src.getPath().toStdString(), dest.getPath().toStdString()};
    Iridium::Utility::pushBack(_args, {getFlag(Transfers).to_vector(), getFlag(Stats).to_vector(),
                                       getFlag(Verbose).to_vector(), getFlag(LogType).to_vector()});

    _readyRead.connect(
            [this](const string &data)
            {
                bj::object json = parseJson(data);
                if (not json.contains("error"))
                {
                    _map_data.clear();
                    _map_data.emplace("json", boost::json::serialize(json));
                    emit readDataJson(std::move(json));
                }
            });
    if (not _lockable)
        execute();
}


/**
 * @brief Rclone::deleteFile, delete a file
 * @param file
 */
void Rclone::deleteFile(const RcloneFile &file)
{
    _args = {file.getPath().toStdString()};
    Iridium::Utility::pushBack(_args, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector()});
    if (!file.isDir())
        _args.emplace(_args.begin(), "deletefile");
    else
        _args.emplace(_args.begin(), "purge");
    if (not _lockable)
        execute();
}


/**
 * @brief Rclone::config, configure a remote
 * @param type
 * @param params
 */
void Rclone::config(RemoteType type, const string &name, const vector<string> &params)
{
    vector<string> args = {"config", "create", name};
    switch (type)
    {
        case Drive:
            args.emplace_back("drive");
            break;
        case Sftp:
            args.emplace_back("sftp");
            break;
        case OneDrive:
            args.emplace_back("onedrive");
            break;
        case Dropbox:
            args.emplace_back("dropbox");
            break;
        case Ftp:
            args.emplace_back("ftp");
            break;
        case Mega:
            args.emplace_back("mega");
            break;
        case OpenDrive:
            args.emplace_back("opendrive");
            break;
        case Pcloud:
            args.emplace_back("pcloud");
            break;
        case Box:
            args.emplace_back("box");
            break;
        case Smb:
            args.emplace_back("smb");
            break;
        default:
            break;
    }
    args.insert(args.end(), params.begin(), params.end());
    _args = args;
    if (not _lockable)
        execute();
}


/**
 * @brief Rclone::about, rclone about command : get info about remote
 * @param info
 */
void Rclone::about(const RemoteInfo &info)
{
    _finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    auto json = parseJson(boost::algorithm::join(_out, ""));
                    _map_data["json"] = boost::json::serialize(json);
                }
            });
    _args = {"about", info.path, "--json"};
    if (not _lockable)
        execute();
}

/**
 * @brief Rclone::lstRemote, return a list of remote
 */
vector<RemoteInfoPtr> Rclone::listRemotes()
{
    _args = {"listremotes", "--long"};
    execute();
    waitForFinished();
    vector<RemoteInfoPtr> remotes;
    for (auto &string: _out)
    {
        auto name_regex = std::regex("([a-zA-Z0-9]+):");
        auto type_regex = std::regex("([a-zA-Z0-9]+)$");
        std::smatch match;
        std::regex_search(string, match, name_regex);
        auto name = match[0].str();
        std::regex_search(string, match, type_regex);
        auto type = match[0].str();
        remotes.emplace_back(std::make_shared<RemoteInfo>(name, stringToRemoteType.find(type)->second));
    }
    return remotes;
}

/**
 * @brief Rclone::search, search files in remote
 * @param filters
 * @param info
 */
void Rclone::search(const vector<Filter> &filters, const RemoteInfo &info)
{
    auto c = _readyRead.connect(
            [this](const string &line)
            {
                try
                {
                    bj::object json;
                    std::regex size_regex("([0-9]+ )");
                    std::regex date_regex("([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{9})");
                    std::smatch match;
                    std::regex_search(line, match, size_regex);
                    if (match.empty())
                        return;
                    json.emplace("Size", std::stoll(match[0].str()));
                    std::regex_search(line, match, date_regex);
                    if (match.empty())
                        return;
                    json.emplace("ModTime", match[0].str());
                    auto path = match.suffix().str().substr(1, match.suffix().str().size() - 1);
                    json.emplace("Path", path);
                    auto name = path.substr(path.find_last_of('/') + 1, path.size() - 1);
                    json.emplace("Name", name);
                    json.emplace("IsDir", false);
                    emit readDataJson(std::move(json));
                } catch (...)
                {
                    std::cerr << " probleme search " << line << std::endl;
                }
            });
    _args = {"lsl", info.path};
    for (auto &filter: filters)
        _args.emplace_back(filter.str());
    _args.emplace_back("--ignore-case");
    if (not _lockable)
        execute();
}

/**
 * @brief Rclone::tree, rclone tree command
 * @param file
 */
void Rclone::tree(const RcloneFile &file)
{
    _args = {"tree", file.getPath().toStdString()};
    if (not _lockable)
        execute();
}


/**
 * @brief Rclone::deleteRemote, delete a remote
 * @param remote
 */
void Rclone::deleteRemote(const string &remote)
{
    _args = {"config", "delete", remote};
    if (not _lockable)
        execute();
}

/**
 * @brief Rclone::execute, launch child process with args
 * @param args
 */
void Rclone::execute()
{
    if (_state == Running)
        throw std::runtime_error("Rclone is running");
    auto rclone_exist = bfs::exists(pathRclone());
    if (not rclone_exist)
    {
        Rclone::check_rclone(false);
        _state = Error;
        _cv.notify_all();
        return;
    }
//    cout << "execute " << boost::algorithm::join(_args, " ") << endl;

#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring exe = converter.from_bytes(pathRclone());
    vector<std::wstring> argsEncoding;
    argsEncoding.reserve(_args.size());
    for (auto &arg: _args)
        argsEncoding.emplace_back(converter.from_bytes(arg));
#else
    auto exe = pathRclone();
    const auto &argsEncoding = _args;
#endif

    _ioc = new ba::io_context();
    _pipe = new bp::async_pipe(*_ioc);

    try
    {
        _child = new bp::child(bp::shell,
                               bp::exe = exe,
                               bp::args = argsEncoding,
                               (bp::std_out & bp::std_err) > *_pipe,
                               bp::std_in.close(),
                               *_ioc
#ifdef _WIN32
                , bp::windows::hide
#endif
        );
        _resetable = true;
    } catch (boost::process::process_error &e)
    {
        cout << e.what() << endl;
        _cv.notify_all();
        _state = Error;
        return;
    }
    emit started();
    _state = Running;
    // notify that the process has started
    _cv.notify_one();


    _thread = make_unique<boost::thread>(
            [this]
            {
                try
                {
                    ba::post(*_ioc, [this] { readLoop(*_pipe, _out); });
                    _ioc->run();
                    _child->wait();
                    _exit = _child->exit_code();
                    _finished(_exit);
                    emit finished(_exit);
                    _state = Finsished;
                    _cv.notify_one();
                    RcloneManager::finished(this);
                    Rclone::reset();
                } catch (boost::exception &e)
                {
                    throw std::runtime_error(boost::diagnostic_information(e));
                }
            });
}

/**
 * @brief Rclone::readLoop, read the output of the process
 * @param pipe
 * @param vec
 */
void Rclone::readLoop(boost::process::async_pipe &pipe, std::vector<std::string> &vec)
{
    ba::async_read_until(
            pipe,
            _buffer,
            '\n',
            [&](boost::system::error_code error_code, std::size_t bytes)
            {
                if (not error_code)
                {
                    string line;
                    if (std::getline(std::istream(&_buffer), line))
                    {
                        _readyRead(line);
                        vec.emplace_back(line);
//                                    cout << line << endl;
                    }
                    readLoop(pipe, vec);
                }
            }
    );
}

/**
 * @brief Rclone::waitForFinished, wait for the process to finish
 */
void Rclone::waitForFinished()
{
    if (_state not_eq Running)
        waitForStarted();
    if (_state == Running)
    {
        unique_lock<mutex> lock(_mutex);
        _cv.wait(lock, [this] { return _state == Finsished or _state == Error; });
    }
}

/**
 * @brief destructeur
 */
Rclone::~Rclone()
{
    // if mutex is not available, wait for it
    lock_guard<mutex> lock(_mutex);
    cout << "destructeur rclone : " << this << endl;
//    Rclone::kill();
    if (_state == Running)
    {
        cerr << "destructeur rclone : " << this << " is running" << endl;
        std::terminate();
    }
}

/**
 * @brief Rclone::kill, kill the process
 */
void Rclone::kill()
{
    if (_state == Running)
    {
        cout << "process rclone kill : " << this << endl;
        emit killed();
        _ioc->stop();
        _child->terminate();
        _thread->join();
        _exit = -1;
    }
    _state = Kill;
    clear();
}

/**
 * @brief Rclone::reset, delete all pointer and disconnect all signal
 */
void Rclone::reset()
{
    cout << "reset rclone : " << this << endl;
    // if process finish and destructor, call reset twice
    lock_guard<mutex> lock(_mutex);
    if (not _resetable)
        return;
    _resetable = false;
    if (_child)
    {
        delete _child;
        _child = nullptr;
    }
    if (_pipe)
    {
        delete _pipe;
        _pipe = nullptr;
    }
    if (_ioc)
    {
        delete _ioc;
        _ioc = nullptr;
    }
    _finished.disconnect_all_slots();
    _readyRead.disconnect_all_slots();
    this->disconnect();
    _aquired = false;
}

/**
 * @brief Rclone::waitForStarted, wait for the process to start
 */
void Rclone::waitForStarted()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this] { return _state == Running or _state == Error; });
}

/**
 * @brief Rclone::size, get size of a file or directory
 * @param path
 */
void Rclone::size(const RcloneFile &path)
{
    _finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    for (const auto &string: _out)
                    {
                        try
                        {
                            auto json = bj::parse(string);
                            auto count = json.at("count").as_int64();
                            auto sizeBytes = json.at("bytes").as_int64();
                            auto humanReadable = Iridium::Utility::sizeToString((double64_t) sizeBytes);
                            emit sizeFinished(count, sizeBytes, std::move(humanReadable));
                        } catch (boost::exception &e)
                        {
                            cerr << "Erreur parse json" << endl;
                            cerr << boost::diagnostic_information(e) << endl;
                        }
                    }
                }
            });
    _args = {"size", path.getPath().toStdString(), "--json"};
    if (not _lockable)
        execute();
}

/**
 * @brief Rclone::version,get rclone version
 * @return version
 */
string Rclone::version()
{
    _args = {"version"};
    if (not _lockable)
        execute();
    waitForFinished();
    return !_out.empty() ? _out.front() : "Unknown";
}


/**
 * @brief Rclone::mkdir, Create a new directory.
 * @param dir
 */
void Rclone::mkdir(const RcloneFile &dir)
{
    _args = {"mkdir", dir.getPath().toStdString()};
    Iridium::Utility::pushBack(
            _args, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    if (not _lockable)
        execute();

}

/**
 * @brief Rclone::moveto, Move file or directory from source to dest.
 * @param src
 * @param dest
 */
void Rclone::moveto(const RcloneFile &src, const RcloneFile &dest)
{
    _args = {"moveto", src.getPath().toStdString(), dest.getPath().toStdString()};
    Iridium::Utility::pushBack(
            _args, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    if (not _lockable)
        execute();
}

/**
 * @brief Parse string to json
 */
bj::object Rclone::parseJson(const string &str)
{
    try
    {
        auto json = bj::parse(str);
        return json.as_object();
    } catch (boost::exception &e)
    {
        cerr << "Erreur parse json" << endl;
        cerr << str << endl;
    }
    auto json = bj::object();
    json.emplace("error", true);
    return json;
}

std::atomic_int_fast8_t RcloneManager::_rclone_locked;
mutex RcloneManager::_launch_mutex;
condition_variable RcloneManager::_launch_cv;
std::vector<RclonePtr> RcloneManager::_launch_queue;
bool RcloneManager::lock_launch;

boost::thread RcloneManager::_launch_thread = boost::thread(
        []
        {
            while (not boost::this_thread::interruption_requested())
            {
                try
                {
                    // wait for rclone process to be less than max
                    boost::this_thread::interruption_point();
                    unique_lock<mutex> lock(RcloneManager::_launch_mutex);
                    RcloneManager::_launch_cv.wait(lock, []
                    {
                        boost::this_thread::interruption_point();
                        return not RcloneManager::_launch_queue.empty() and
                               RcloneManager::_rclone_locked < maxProcess() and not RcloneManager::lock_launch;
                    });
                    if (not RcloneManager::_launch_queue.empty())
                    {
                        auto rclone = RcloneManager::_launch_queue.front();
                        RcloneManager::_launch_queue.erase(RcloneManager::_launch_queue.begin());
                        rclone->execute();
                        ++RcloneManager::_rclone_locked;
                    }
                } catch (boost::thread_interrupted &e) { break; }
            }
        });

/**
 * @brief RcloneManager::launch, add a rclone process to the launch queue
 * @param rcloneLocked
 */
void RcloneManager::addLockable(const RclonePtr &rcloneLocked)
{
    _launch_queue.insert(_launch_queue.end(), rcloneLocked);
    _launch_cv.notify_one();
}

/**
 * @brief RcloneManager::finished, rclone process call this function when finished
 */
void RcloneManager::finished(Rclone *rclone)
{
    if (rclone->_lockable)
    {
        --RcloneManager::_rclone_locked;
        RcloneManager::_launch_cv.notify_one();
    }
}