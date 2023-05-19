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
                        auto array = boost::json::parse(joinStr).as_array();
                        emit lsJsonFinished(std::move(array));
                    } catch (boost::exception &e)
                    {
                        emit lsJsonFinished(std::move(bj::array()));
                        cerr << boost::algorithm::join(m_out, "") << endl;
                        cerr << "Error: " << boost::diagnostic_information(e) << endl;
                    }
                }
            });
    _args = {"lsjson", path, "--drive-skip-gdocs", "--fast-list"};
    if (not m_lockable)
        execute();
}

/**
 * @brief Upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::copyTo(const RcloneFile &src, const RcloneFile &dest)
{
    _args =
            {"copyto", src.getPath().toStdString(), dest.getPath().toStdString()};
    Iridium::Utility::pushBack(_args, {getFlag(Transfers).to_vector(), getFlag(Stats).to_vector(),
                                       getFlag(Verbose).to_vector(), getFlag(LogType).to_vector()});

    m_readyRead.connect(
            [this](const string &data)
            {
                bj::object json = parseJson(data);
                if (not json.contains("error"))
                {
                    m_map_data.clear();
                    m_map_data.emplace("json", boost::json::serialize(json));
                    emit taskProgress(std::move(json));
                }
            });
    if (not m_lockable)
        execute();
}


/**
 * @brief Rclone::deleteFile, supprime un fichier
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
    if (not m_lockable)
        execute();
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
    _args = args;
    if (not m_lockable)
        execute();
}

/**
 * @brief Rclone::about, permet de récupérer des informations sur un remote
 * @param info
 */
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
    _args = {"about", info.m_path, "--json"};
    if (not m_lockable)
        execute();
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
    _args = {"listremotes", "--long"};
    if (not m_lockable)
        execute();
}

/**
 * @brief Rclone::search, permet de rechercher des fichiers dans un remote
 * @param filters
 * @param info
 */
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
                    emit searchRefresh(std::move(json));
                } catch (...)
                {
                    std::cerr << " probleme search " << line << std::endl;
                }
            });
    _args = {"lsl", info.m_path};
    for (auto &filter: filters)
        _args.emplace_back(filter.str());
    _args.emplace_back("--ignore-case");
    if (not m_lockable)
        execute();
}


/**
 * @brief Rclone::deleteRemote, permet de supprimer un remote
 * @param remote
 */
void Rclone::deleteRemote(const string &remote)
{
    _args = {"config", "delete", remote};
    if (not m_lockable)
        execute();
}

/**
 * @brief Rclone::execute, execute la commande m_rclone avec les arguments args
 * @param args
 */
void Rclone::execute()
{
    if (m_state not_eq NotLaunched)
    {
        throw std::runtime_error("Rclone is not reusable");
    }
    if (m_cancel)
        return;
//    cout << "execute " << boost::algorithm::join(_args, " ") << endl;

#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring exe = converter.from_bytes(m_path_rclone);
    vector<std::wstring> argsEncoding;
    argsEncoding.reserve(_args.size());
    for (auto &arg: _args)
        argsEncoding.emplace_back(converter.from_bytes(_args));
#else
    auto exe = m_path_rclone;
    const auto &argsEncoding = _args;
#endif

    m_ioc = make_unique<boost::asio::io_context>();
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work(m_ioc->get_executor());
    m_pipe_err = make_unique<bp::async_pipe>(*m_ioc);
    m_pipe_out = make_unique<bp::async_pipe>(*m_ioc);

    m_child = bp::child(exe, bp::args(argsEncoding),
                        bp::std_out > *m_pipe_out,
                        bp::std_err > *m_pipe_err,
                        bp::on_exit([this](int exit, const std::error_code &ec)
                                    {
                                        m_exit = exit;
                                    }), *m_ioc
#ifdef _WIN32
            , bp::windows::hide
#endif
    );
    emit started();
    m_state = Running;
    // notify that the process has started
    m_cv.notify_one();

    auto buffer = make_shared<ba::streambuf>();
    read_loop =
            [this, buffer](bp::async_pipe &pipe, vector<string> &vec)
            {
                ba::async_read_until(
                        pipe,
                        *buffer,
                        '\n',
                        [&](boost::system::error_code error_code, std::size_t bytes)
                        {
                            if (not error_code)
                            {
                                string line;
                                if (std::getline(std::istream(&(*buffer)), line))
                                {
                                    m_readyRead(line);
                                    vec.emplace_back(line);
//                                    cout << line << endl;
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
                    m_child.wait();
                    m_state = Finsished;
                    m_finished(m_exit);
                    emit finished(m_exit);
                    m_cv.notify_one();
                    m_pipe_out->close();
                    m_pipe_err->close();
                    RcloneManager::finished(this);
                } catch (...)
                {
                    cerr << "Exception in thread: " << endl;
                    m_finished(m_exit);
                    emit finished(m_exit);
                    m_cv.notify_one();
                    m_pipe_out->close();
                    m_pipe_err->close();
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
    Rclone::kill();
}

/**
 * @brief Rclone::kill, tue le processus rclone
 */
void Rclone::kill()
{
    if (m_state == Running)
    {
        cout << "process rclone kill" << endl;
        m_readyRead.disconnect_all_slots();
        m_finished.disconnect_all_slots();
        m_ioc->stop();
        m_child.detach();
        bp::detail::api::terminate(bp::child::child_handle{m_child.native_handle()});
        m_thread->interrupt();
        m_exit = 1;
        disconnect();
    }
        m_state = Stopped;
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
    _args = {"size", path, "--json"};
    if (not m_lockable)
        execute();
}

/**
 * @brief retourne la version de rclone
 * @return version
 */
string Rclone::version()
{
    _args = {"version"};
    if (not m_lockable)
        execute();
    waitForFinished();
    return m_out[0];
}


/**
 * @brief mkdir, crée un dossier
 * @param dir
 */
void Rclone::mkdir(const RcloneFile &dir)
{
    _args = {"mkdir", dir.getPath().toStdString()};
    Iridium::Utility::pushBack(
            _args, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    if (not m_lockable)
        execute();

}

/**
 * @brief deplace un fichier src vers dest
 * @param src
 * @param dest
 */
void Rclone::moveto(const RcloneFile &src, const RcloneFile &dest)
{
    _args = {"moveto", src.getPath().toStdString(), dest.getPath().toStdString()};
    Iridium::Utility::pushBack(
            _args, {getFlag(Verbose).to_vector(), getFlag(LogType).to_vector(), getFlag(Stats).to_vector()});
    if (not m_lockable)
        execute();
}

/**
 * @brief Parse une chaine de caractère en json
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

std::atomic_int_fast8_t RcloneManager::m_nb_rclone_locked;
uint8_t RcloneManager::m_nb_max_process = thread::hardware_concurrency();
mutex RcloneManager::m_launch_mutex;
condition_variable RcloneManager::m_launch_cv;
std::vector<RclonePtr> RcloneManager::m_launch_queue;
std::vector<RclonePtr> RcloneManager::m_rclones;

boost::thread RcloneManager::m_launch_thread = boost::thread(
        []
        {
            while (not boost::this_thread::interruption_requested())
            {
                try
                {
                    // On attend que le nombre de processus rclone soit inférieur au nombre max
                    boost::this_thread::interruption_point();
                    unique_lock<mutex> lock(RcloneManager::m_launch_mutex);
                    RcloneManager::m_launch_cv.wait(lock, []
                    {
                        boost::this_thread::interruption_point();
                        return not RcloneManager::m_launch_queue.empty() and
                               RcloneManager::m_nb_rclone_locked < RcloneManager::m_nb_max_process;
                    });
                    if (not RcloneManager::m_launch_queue.empty())
                    {
                        auto rclone = RcloneManager::m_launch_queue.front();
                        release(rclone);
                        if (not rclone->isCanceled())
                        {
                            rclone->execute();
                            ++RcloneManager::m_nb_rclone_locked;
                        }
                    }
                } catch (boost::thread_interrupted &e) { break; }
            }
        });

/**
 * @brief RcloneManager::get, retourne un pointeur vers un rclone
 * @return un pointeur vers un rclone
 */
RclonePtr RcloneManager::get(bool lockable)
{
    for (auto rclone: m_rclones)
    {
        if (rclone->isCanceled() and rclone.use_count() == 1)
        {
            rclone->m_lockable = lockable;
            rclone->m_cancel = false;
            return rclone;
        }
    }
    auto rclone = RclonePtr(new Rclone(lockable));
    m_rclones.push_back(rclone);
    return rclone;
}

/**
 * @brief RcloneManager::launch, ajout le processus rclone à la queue de lancement
 * @param rcloneLocked
 */
void RcloneManager::addLockable(const RclonePtr &rcloneLocked)
{
    m_launch_queue.insert(m_launch_queue.end(), rcloneLocked);
    m_launch_cv.notify_one();
}

/**
 * @brief RcloneManager::finished, un processus rclone appel cette fonction lorsqu’il se termine, pour notifier le manager
 */
void RcloneManager::finished(Rclone *rclone)
{
    if (rclone->m_lockable)
    {
        --RcloneManager::m_nb_rclone_locked;
        RcloneManager::m_launch_cv.notify_one();
    } else release(rclone);
}