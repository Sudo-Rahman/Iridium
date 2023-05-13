//
// Created by Rahman on 09/01/2023.
//

#include <Rclone.hpp>

#include <iostream>

#include <Utility/Utility.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/chrono.hpp>
#include <regex>
#include <boost/asio.hpp>


#ifdef _WIN32
#include <boost/process/windows.hpp>
#include <codecvt>
#endif

namespace bp = boost::process;
namespace bj = boost::json;
namespace ba = boost::asio;
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
                        cerr << boost::algorithm::join(m_out, "") << endl;
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
                    emit taskProgress(std::move(json));
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
                        auto name_regex = std::regex("([a-zA-Z0-9]+):");
                        auto type_regex = std::regex("([a-zA-Z0-9]+)$");
                        std::smatch match;
                        std::regex_search(string, match, name_regex);
                        auto name = match[0].str();
                        std::regex_search(string, match, type_regex);
                        auto type = match[0].str();
                        map.insert({name, type});
                    }
                    m_map_data = map;
                }
            });
    execute({"listremotes", "--long"});
}

void Rclone::search(const vector<Filter> &filters, const RemoteInfo &info)
{
    m_readyRead.connect(
            [this](const string &line)
            {
                try
                {
                    bj::object json;
                    std::regex size_regex("([0-9]+ )");
                    std::regex date_regex("([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{9})");
                    std::smatch match;
                    std::regex_search(line, match, size_regex);
                    json.emplace("Size", std::stoll(match[0].str()));
                    std::regex_search(line, match, date_regex);
                    json.emplace("ModTime", match[0].str());
                    auto path = match.suffix().str().substr(1, match.suffix().str().size() - 1);
                    json.emplace("Path", path);
                    auto name = path.substr(path.find_last_of('/') + 1, path.size() - 1);
                    json.emplace("Name", name);
                    json.emplace("IsDir", false);
                    emit searchRefresh(std::move(json));
                } catch (std::out_of_range &e)
                {
                    std::cerr << e.what() << " probleme search " << line << std::endl;
                }
            });
    vector<string> args = {"lsl", info.m_path};
    for (auto &filter: filters)
        args.emplace_back(filter.str());
    args.emplace_back("--ignore-case");
    execute(args);
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
    if (m_state not_eq NotLaunched)
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
    emit started();
    m_state = Running;

    m_ioc = make_unique<boost::asio::io_context>();
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work(m_ioc->get_executor());
    m_pipe_err = make_unique<bp::async_pipe>(*m_ioc);
    m_pipe_out = make_unique<bp::async_pipe>(*m_ioc);

    m_child = bp::child(exe, bp::args(argsEncoding),
                        bp::std_out > *m_pipe_out,
                        bp::std_err > *m_pipe_err,
                        bp::on_exit([this](int exit, const std::error_code &ec)
                                    {
                                        m_state = Finsished;
                                        m_ioc->stop();
                                        m_exit = exit;
                                        m_finished(m_exit);
                                        emit finished(m_exit);
                                        m_cv.notify_one();
                                    }), *m_ioc
#ifdef _WIN32
            , bp::windows::hide
#endif
    );
    // notify that the process has started
    m_cv.notify_one();

    read_loop =
            [this](bp::async_pipe &pipe, vector<string> &vec)
            {
                static boost::asio::streambuf buffer;
                ba::async_read_until(
                        pipe,
                        buffer,
                        '\n',
                        [&](boost::system::error_code error_code, std::size_t bytes)
                        {
                            if (not error_code)
                            {
                                istream is(&buffer);
                                string line;
                                if (std::getline(is, line))
                                {
                                    m_readyRead(line);
                                    vec.emplace_back(line);
                                }
                                read_loop(pipe, vec);
                            }
                        }
                );
            };

    m_thread = make_unique<boost::thread>(
            [this]
            {
                try
                {
                    ba::post(*m_ioc, [this]
                    {
                        read_loop(*m_pipe_out, m_out);
                        read_loop(*m_pipe_err, m_error);
                    });
                    m_ioc->run();
                    m_pipe_out->close();
                    m_pipe_err->close();
                    RcloneManager::finished(this);
                } catch (...)
                {
                    cerr << "Exception in thread: " << endl;
                    m_pipe_out->close();
                    m_pipe_err->close();
                    m_ioc->stop();
                    RcloneManager::finished(this);
                }
            });
}

/**
 * @brief Rclone::waitForFinished, attend que le processus rclone se termine
 */
void Rclone::waitForFinished()
{
    if (m_state not_eq Running)
        waitForStarted();
    if (m_state == Running)
    {
        unique_lock<mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_state == Finsished; });
    }
}

/**
 * @brief destructeur
 */
Rclone::~Rclone()
{
    cout << "destructeur rclone" << endl;
    if (m_state == Running)
        terminate();
    else
    {
        m_ioc->stop();
    }
}

/**
 * @brief Rclone::terminate, termine le processus rclone
 */
void Rclone::kill()
{
    if (m_state == Running)
    {
        cout << "process rclone kill" << endl;
        m_child.detach();
        bp::detail::api::terminate(bp::child::child_handle{m_child.native_handle()});
        m_thread->interrupt();
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
                            emit sizeFinished(count, sizeBytes, std::move(humanReadable));
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
    vector<string> arguments = {"mkdir", dir.getPath().toStdString()};
    Iridium::Utility::pushBack(
            arguments, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    execute(arguments);

}

void Rclone::moveto(const RcloneFile &src, const RcloneFile &dest)
{
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
        cerr << str << endl;
    }
    auto json = bj::object();
    json.emplace("error", true);
    return json;
}

std::atomic_int_fast8_t RcloneManager::m_nb_rclone_locked;
uint8_t RcloneManager::m_nb_max_process = thread::hardware_concurrency();
mutex RcloneManager::m_launch_mutex;
condition_variable RcloneManager::m_launch_cv;
std::vector<RcloneLocked> RcloneManager::m_launch_queue;
std::vector<RclonePtr> RcloneManager::m_rclones;

boost::thread RcloneManager::m_launch_thread = boost::thread(
        []
        {
            while (not boost::this_thread::interruption_requested())
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
                    RcloneManager::m_launch_queue.erase(RcloneManager::m_launch_queue.begin());
                    if (not rclone_locked.isCanceled())
                    {
                        rclone_locked.func();
                        ++RcloneManager::m_nb_rclone_locked;
                    }
                }
            }
        });

/**
 * @brief RcloneManager::get, retourne un pointeur vers un rclone
 * @return un pointeur vers un rclone
 */
RclonePtr RcloneManager::get(bool lockable)
{
    auto rclone = RclonePtr(new Rclone());
    m_rclones.push_back(rclone);
    rclone->m_lockable = lockable;
    return rclone;
}

void RcloneManager::launch(const RcloneLocked &rcloneLocked)
{
    m_launch_queue.insert(m_launch_queue.end(), rcloneLocked);
    m_launch_cv.notify_one();
}

/**
 * @brief RcloneManager:: finished, un processus rclone appel cette fonction lorsqu’il se termine, pour notifier le manager
 */
void RcloneManager::finished(Rclone *rclone)
{
    if (rclone->m_lockable)
    {
        --RcloneManager::m_nb_rclone_locked;
        RcloneManager::m_launch_cv.notify_one();
    }
    m_rclones.erase(std::remove_if(m_rclones.begin(), m_rclones.end(), [rclone](const RclonePtr &rclonePtr)
    {
        return rclonePtr.get() == rclone;
    }), m_rclones.end());
}