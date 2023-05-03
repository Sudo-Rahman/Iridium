//
// Created by Rahman on 09/01/2023.
//

#include <Rclone.hpp>

#include <utility>
#include <iostream>

#include <Utility/Utility.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/chrono.hpp>


#ifdef _WIN32
#include <boost/process/windows.hpp>
#include <codecvt>
#endif

namespace bp = boost::process;
namespace bj = boost::json;
using namespace std;

std::map<Rclone::Flag, Rclone::flags_str> Rclone::m_map_flags{
        {Rclone::Flag::Transfers, {"--transfers", "4"}},
        {Rclone::Flag::Stats,     {"--stats",     "0.5s"}},
        {Rclone::Flag::Retries,   {"--retries",   "3"}},
        {Rclone::Flag::Verbose,   {"",            "-v"}},
        {Rclone::Flag::LogType,   {"",            "--use-json-log"}},
};


/**
 * @brief Rclone::setPathRclone
 * @param pathRclone
 */
void Rclone::setPathRclone(const string &pathRclone)
{
    Rclone::m_path_rclone = pathRclone;
}

/**
 * @brief Liste sous forme de json les fichiers dans le dossier path
 * @param path
 */
void Rclone::lsJson(const string &path)
{
    m_finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    try
                    {
                        auto joinStr = boost::algorithm::join(m_out, "");
                        lsJsonFinished(QJsonDocument::fromJson(joinStr.c_str()));
                    } catch (boost::exception &e)
                    {
                        emit lsJsonFinished(QJsonDocument());
                        cerr << "Error: " << boost::diagnostic_information(e) << endl;
                    }
                }
            });
    execute({"lsjson", path, "--drive-skip-gdocs", "--fast-list"});
}

/**
 * @brief Upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::copyTo(const RcloneFile &src, const RcloneFile &dest)
{
    connectTaskSignalFinishedJson();
    vector<string> arguments(
            {"copyto", src.getPath().toStdString(), dest.getPath().toStdString()});
    Iridium::Utility::pushBack(arguments, {getFlag(Transfers).to_vector(), getFlag(Stats).to_vector(),
                                           getFlag(Verbose).to_vector(), getFlag(LogType).to_vector()});

    m_readyRead.connect(
            [this](const string &data)
            {
                bj::object json = parseJson(data);
                if (not json.contains("error"))
                {
                    emit taskProgress(json);
                    m_map_data.clear();
                    m_map_data.emplace("json", boost::json::serialize(json));
                }
            });
    execute(arguments);
}


/**
 * @brief Rclone::deleteFile, supprime un fichier
 * @param file
 */
void Rclone::deleteFile(const RcloneFile &file)
{
    vector<string> arguments({file.getPath().toStdString()});
    Iridium::Utility::pushBack(arguments, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector()});
    connectTaskSignalFinishedJson();
    if (!file.isDir())
        arguments.emplace(arguments.begin(), "deletefile");
    else
        arguments.emplace(arguments.begin(), "purge");
    execute(arguments);
}


/**
 * @brief Rclone::config, permet de configurer un nouveau remote
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
    execute(args);
}

void Rclone::about(const RemoteInfo &info)
{
    m_finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    auto json = parseJson(boost::algorithm::join(m_out, ""));
                    m_map_data["json"] = boost::json::serialize(json);
                }
            });
    execute({"about", info.m_path, "--json"});
}

/**
 * @brief Rclone::lstRemote, permet de lister les remotes configurés
 */
void Rclone::listRemotes()
{
    m_finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    map<string, string> map;
                    for (auto &string: m_out)
                    {
                        auto str = QString::fromStdString(string);
                        auto name = str.split(":")[0].toStdString();
                        auto type = str.split(":")[1].toStdString();
                        type.erase(remove(type.begin(), type.end(), ' '), type.end());
                        map.insert({name, type});
                    }
                    m_map_data = map;
                }
            });
    execute({"listremotes", "--long"});
}

/**
 * @brief Rclone::deleteRemote, permet de supprimer un remote
 * @param remote
 */
void Rclone::deleteRemote(const string &remote)
{
    execute({"config", "delete", remote});
}

/**
 * @brief Rclone::execute, execute la commande m_rclone avec les arguments args
 * @param args
 */
void Rclone::execute(const vector<string> &args)
{
    if (m_state == Stopped or m_state == Finsished)
    {
        std::exception_ptr eptr = std::make_exception_ptr(
                std::runtime_error("Rclone is not reusable"));
    }

#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring exe = converter.from_bytes(m_path_rclone);
    vector<std::wstring> argsEncoding;
    argsEncoding.reserve(args.size());
    for (auto &arg: args)
        argsEncoding.emplace_back(converter.from_bytes(arg));
#else
    auto exe = m_path_rclone;
    const auto &argsEncoding = args;
#endif
    m_thread = std::make_shared<boost::thread>(
            [this, exe, argsEncoding]
            {
                emit started();
                m_state = Running;
                m_out.clear();
                bp::ipstream out;
                bp::ipstream err;
                m_child = bp::child(exe, bp::args(argsEncoding),
                                    bp::std_out > out,
                                    bp::std_err > err
#ifdef _WIN32
                        , bp::windows::hide
#endif
                );

                // notify that the process has started
                m_cv.notify_one();

                string line_out, line_err;
                auto th1 = boost::thread(
                        [this, &err, &line_err]
                        {
                            while (getline(err, line_err))
                            {
                                if (m_error.size() > 1000)
                                    m_error.clear();
                                m_error.emplace_back(line_err);
                                m_readyRead(line_err);
                            }
                        });
                auto th2 = boost::thread(
                        [this, &out, &line_out]
                        {
                            while (getline(out, line_out))
                            {
                                if (m_out.size() > 1000)
                                    m_out.clear();
                                m_out.emplace_back(line_out);
                                m_readyRead(line_out);
                            }
                        });
                m_child.wait();
                th1.join();
                th2.join();
                m_exit = m_child.exit_code();
                m_finished(m_exit);
                emit finished(m_exit);
                m_state = Finsished;
                if (m_lockable) { RcloneManager::finished(this); }
            });
}

/**
 * @brief Rclone::waitForFinished, attend que le processus rclone se termine
 */
void Rclone::waitForFinished()
{
    if (m_state not_eq Running)
        waitForStarted();
    if (m_state == Running and m_thread->joinable())
        m_thread->join();
}

/**
 * @brief destructeur
 */
Rclone::~Rclone()
{
    terminate();
}

/**
 * @brief Rclone::terminate, termine le processus rclone
 */
void Rclone::terminate()
{
    if (m_state == Running)
    {
        cout << "process rclone kill" << endl;
        m_child.detach();
#ifdef _WIN32
        const auto explorer = OpenProcess(PROCESS_TERMINATE, false, m_child.id());
        TerminateProcess(explorer, 1);
        CloseHandle(explorer);
#else
        kill(m_child.id(), SIGKILL);
#endif
        if (m_thread->joinable())
            m_thread->join();
        m_exit = 1;
    }
    m_state = Stopped;
}

/**
 * @brief Rclone::state, retourne l'état du processus m_rclone
 * @return
 */
Rclone::State Rclone::state() const
{
    return m_state;
}

/**
 * @brief Rclone::waitForStarted, attend que le processus m_rclone démarre
 */
void Rclone::waitForStarted()
{
    while (m_state != Running)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock);
    }
}

/**
 * @brief Rclone::size, retourne la taille d'un fichier ou d'un dossier
 * @param path
 */
void Rclone::size(const string &path)
{
    m_finished.connect(
            [this](const int exit)
            {
                if (exit == 0)
                {
                    for (const auto &string: m_out)
                    {
                        try
                        {
                            auto json = bj::parse(string);
                            auto count = json.at("count").as_int64();
                            auto sizeBytes = json.at("bytes").as_int64();
                            auto humanReadable = Iridium::Utility::sizeToString((double64_t) sizeBytes);
                            emit sizeFinished(count, sizeBytes, humanReadable);
                        } catch (boost::exception &e)
                        {
                            cerr << "Erreur parse json" << endl;
                            cerr << boost::diagnostic_information(e) << endl;
                        }
                    }
                }
            });
    execute({"size", path, "--json"});

}

string Rclone::version()
{
    execute({"version"});
    waitForFinished();
    return m_out[0];
}

map<string, string> Rclone::getData() const
{
    return m_map_data;
}

std::vector<std::string> Rclone::readAllError() const
{
    return m_error;
}

uint8_t Rclone::exitCode() const
{
    return m_exit;
}

/**
 * @brief mkdir, crée un dossier
 * @param dir
 */
void Rclone::mkdir(const RcloneFile &dir)
{
    connectTaskSignalFinishedJson();
    vector<string> arguments = {"mkdir", dir.getPath().toStdString()};
    Iridium::Utility::pushBack(
            arguments, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    execute(arguments);

}

void Rclone::moveto(const RcloneFile &src, const RcloneFile &dest)
{
    connectTaskSignalFinishedJson();
    vector<string> arguments = {"moveto", src.getPath().toStdString(), dest.getPath().toStdString()};
    Iridium::Utility::pushBack(
            arguments, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    execute(arguments);
}

bj::object Rclone::parseJson(const string &str)
{
    try
    {
        auto json = bj::parse(str);
        return json.as_object();
    } catch (boost::exception &e)
    {
        cerr << "Erreur parse json" << endl;
    }
    auto json = bj::object();
    json.emplace("error", true);
    return json;
}

void Rclone::connectTaskSignalFinishedJson()
{
    m_finished.connect(
            [this](int exit)
            {
                bj::object json;
                if (exit == 0)
                {
                    auto out = m_out;
                    auto err = m_error;
                    out.insert(out.end(), err.begin(), err.end());
                    json = parseJson(out.back());
                    if (not json.contains("error"))
                    {
                        m_map_data.clear();
                        m_map_data.emplace("json", boost::json::serialize(json));
                    }
                } else
                {
                    json = bj::object();
                    json.emplace("error", m_error.back());
                }
                emit taskFinished(exit, json);
            });
}

std::atomic_int_fast8_t RcloneManager::m_nb_rclone_locked;
uint8_t RcloneManager::m_nb_max_process = thread::hardware_concurrency();
mutex RcloneManager::m_launch_mutex, RcloneManager::m_stop_mutex, RcloneManager::m_mutex_start;
condition_variable RcloneManager::m_launch_cv, RcloneManager::m_stop_cv;
deque<RclonePtr> RcloneManager::m_rclone_vector;
deque<RcloneLocked> RcloneManager::m_launch_queue, RcloneManager::m_stop_queue;

boost::thread RcloneManager::m_launch_thread = boost::thread(
        []
        {
            while (true)
            {
                // On attend que le nombre de processus rclone soit inférieur au nombre max
                unique_lock<mutex> lock(RcloneManager::m_launch_mutex);
                RcloneManager::m_launch_cv.wait(lock, []
                {
                    return not RcloneManager::m_launch_queue.empty() and
                           RcloneManager::m_nb_rclone_locked < RcloneManager::m_nb_max_process;
                });
                if (not RcloneManager::m_launch_queue.empty())
                {
                    auto rclone_locked = RcloneManager::m_launch_queue.front();
                    rclone_locked.func();
                    ++RcloneManager::m_nb_rclone_locked;
                    std::erase_if(RcloneManager::m_launch_queue, [rclone_locked](const RcloneLocked &rclone)
                    {
                        return rclone.rclone == rclone_locked.rclone;
                    });
                }
            }
        });

boost::thread RcloneManager::m_stop_thread = boost::thread(
        []
        {
            while (true)
            {
                // On attend que le nombre de processus rclone soit inférieur au nombre max
                unique_lock<mutex> lock(RcloneManager::m_stop_mutex);
                RcloneManager::m_stop_cv.wait(lock, []
                {
                    return !RcloneManager::m_stop_queue.empty();
                });
                for (auto it = RcloneManager::m_stop_queue.begin(); it not_eq RcloneManager::m_stop_queue.end(); ++it)
                {
                    auto obj = *it;
                    if (obj.rclone->state() not_eq Rclone::Running)
                        obj.rclone->terminate();
                    erase_if(RcloneManager::m_rclone_vector,
                             [obj](const RclonePtr &rclone) { return rclone == obj.rclone; });
                    erase_if(RcloneManager::m_launch_queue,
                             [obj](const RcloneLocked &rcloneLocked) { return rcloneLocked.rclone == obj.rclone; });
                    RcloneManager::m_stop_queue.erase(it);
                    RcloneManager::m_launch_cv.notify_one();
                }
            }
        });


/**
 * @brief RcloneManager::get, retourne un pointeur vers un rclone
 * @return un pointeur vers un rclone
 */
RclonePtr RcloneManager::get()
{
    auto rclone = RclonePtr(new Rclone());
    RcloneManager::m_rclone_vector.push_back(rclone);
    return rclone;
}

void RcloneManager::launch(const RcloneLocked &rcloneLocked)
{
    m_launch_queue.push_back(rcloneLocked);
    m_launch_cv.notify_one();
}

/**
 * @brief RcloneManager:: finished, un processus rclone appel cette fonction lorsqu’il se termine, pour notifier le manager
 */
void RcloneManager::finished(const Rclone *rclone)
{
    --RcloneManager::m_nb_rclone_locked;
    auto it = std::find_if(m_rclone_vector.begin(), m_rclone_vector.end(),
                           [rclone](const RclonePtr &rclonePtr) { return rclonePtr.get() == rclone; });
    if (it not_eq m_rclone_vector.end())
        m_rclone_vector.erase(it);
    RcloneManager::m_launch_cv.notify_one();
}

/**
 * @brief RcloneManager::release, libère un processus rclone
 * @param rclone
 */
void RcloneManager::release(const RclonePtr &rclone)
{
    // erase rclone from vector
    rclone->terminate();
    for (auto it = m_rclone_vector.begin(); it != m_rclone_vector.end(); ++it)
    {
        if (it->get() == rclone.get())
        {
            m_rclone_vector.erase(it);
            break;
        }
    }
}

void RcloneManager::stop(const std::deque<RcloneLocked> &lst)
{
    m_stop_queue.insert(m_stop_queue.end(), lst.begin(), lst.end());
    m_stop_cv.notify_one();
}