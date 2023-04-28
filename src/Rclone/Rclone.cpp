//
// Created by sr-71 on 09/01/2023.
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

std::map<Rclone::Flags, std::string> Rclone::m_mapFlags{
	{Rclone::Flags::Transfers, "4"}
};


/**
 * @brief Rclone::setPathRclone
 * @param pathRclone
 */
void Rclone::setPathRclone(const string &pathRclone)
{
	Rclone::m_pathRclone = pathRclone;
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
		{"copyto", src.getPath().toStdString(), dest.getPath().toStdString(),
		 "-v", "--use-json-log", "--stats", "0.1s", "--transfers", getFlag(Flags::Transfers)});

	m_readyRead.connect(
		[this](const string &data)
		{
			bj::object json = parseJson(data);
			if (not json.contains("error"))
			{
				emit taskProgress(json);
				m_mapData.clear();
				m_mapData.emplace("json", boost::json::serialize(json));
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
	connectTaskSignalFinishedJson();
	if (!file.isDir())
		execute({"deletefile", file.getPath().toStdString(), "-v", "--use-json-log", "--stats", "0.1s"});
	else
		execute({"purge", file.getPath().toStdString(), "-v", "--use-json-log", "--stats", "0.1s"});
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
		default:
			break;
	}
	args.insert(args.end(), params.begin(), params.end());
	execute(args);
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
				m_mapData = map;
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
	std::wstring exe = converter.from_bytes(m_pathRclone);
	vector<std::wstring> argsEncoding;
	argsEncoding.reserve(args.size());
	for (auto &arg: args)
		argsEncoding.emplace_back(converter.from_bytes(arg));
#else
	auto exe = m_pathRclone;
	const auto &argsEncoding = args;
#endif
	m_thread = std::make_shared<boost::thread>(
		[this, exe, argsEncoding]
		{
			RcloneManager::start();
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
			RcloneManager::finished();
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
						auto humanReadable = Iridium::Utility::sizeToString(sizeBytes);
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
	return m_mapData;
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
	execute({"mkdir", dir.getPath().toStdString(), "-v", "--use-json-log", "--stats", "0.1s"});
}

void Rclone::moveto(const RcloneFile &src, const RcloneFile &dest)
{
	connectTaskSignalFinishedJson();
	execute({"moveto", src.getPath().toStdString(), dest.getPath().toStdString(), "-v", "--use-json-log", "--stats",
			 "0.1s"});
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
					m_mapData.clear();
					m_mapData.emplace("json", boost::json::serialize(json));
				}
			} else
			{
				json = bj::object();
				json.emplace("error", m_error.back());
			}
			emit taskFinished(exit, json);
		});
}

atomic_uint8_t RcloneManager::m_nbRclone;
uint8_t RcloneManager::m_nbMaxProcess = thread::hardware_concurrency();
mutex RcloneManager::m_mutex;
condition_variable RcloneManager::m_cv;
vector<RclonePtr> RcloneManager::m_rcloneVector;


/**
 * @brief RcloneManager::get, retourne un pointeur vers un rclone
 * @return un pointeur vers un rclone
 */
RclonePtr RcloneManager::get()
{
	auto rclone = RclonePtr(new Rclone);
	RcloneManager::m_rcloneVector.push_back(rclone);
	return rclone;
}

/**
 * @brief RcloneManager::start, un processus rclone appel cette fonction pour notifier le manager qu'il démarre
 */
void RcloneManager::start()
{
	while (RcloneManager::m_nbRclone >= RcloneManager::m_nbMaxProcess)
	{
		std::unique_lock<std::mutex> lock(RcloneManager::m_mutex);
		RcloneManager::m_cv.wait(lock);
	}
	RcloneManager::m_nbRclone++;
}

/**
 * @brief RcloneManager:: finished, un processus rclone appel cette fonction lorsqu’il se termine, pour notifier le manager
 */
void RcloneManager::finished()
{
	RcloneManager::m_nbRclone--;
	RcloneManager::m_cv.notify_one();
}

/**
 * @brief RcloneManager::release, libère un processus rclone
 * @param rclone
 */
void RcloneManager::release(const RclonePtr &rclone)
{
	// erase rclone from vector
	rclone->terminate();
	RcloneManager::m_rcloneVector.erase(
		std::remove(RcloneManager::m_rcloneVector.begin(), RcloneManager::m_rcloneVector.end(), rclone),
		RcloneManager::m_rcloneVector.end());
}