//
// Created by sr-71 on 09/01/2023.
//

#include <Rclone.hpp>

#include <utility>
#include <iostream>

#include <Utility/Utility.hpp>
#include <boost/algorithm/string/join.hpp>

#ifdef _WIN32
#include <boost/process/windows.hpp>
#endif

namespace bp = boost::process;
namespace bj = boost::json;
using namespace std;

/**
 * @brief Rclone::Rclone
 * @param path
 * @param parent
 */
Rclone::Rclone(string path, RclonesManager *parent) : manager(parent)
{
	Rclone::m_pathRclone = std::move(path);
}

/**
 * @brief Rclone::Rclone
 * @param parent
 */
Rclone::Rclone(RclonesManager *parent) : manager(parent)
{}

/**
 * @brief Rclone::getPathRclone
 */
const string &Rclone::getPathRclone()
{
	return m_pathRclone;
}

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
		 "-v", "--use-json-log", "--stats", "0.1s"});

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
		execute({"purge", file.getPath().toStdString(), "-v","--use-json-log", "--stats", "0.1s"});
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
		default:
			break;
	}
	args.insert(args.end(), params.begin(), params.end());
	execute(args);
}

/**
 * @brief Rclone::m_lstRemote, permet de lister les remotes configurés
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
	qDebug() << args;
	mthread = std::make_shared<boost::thread>(
		[this, args]
		{
			if (manager != nullptr)
				manager->start();
			m_out.clear();
			bp::ipstream out;
			bp::ipstream err;
			auto process = bp::child(m_pathRclone, bp::args(args),
									 bp::std_out > out,
									 bp::std_err > err
#ifdef _WIN32
				, bp::windows::hide
#endif
			);

			mstate = Running;

			v.notify_one();

			string line_out, line_err;
			auto th1 = boost::thread(
				[this, &err, &line_err]
				{
					while (getline(err, line_err))
					{
						if(m_error.size() > 1000)
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
						if(m_out.size() > 1000)
							m_out.clear();
						m_out.emplace_back(line_out);
						m_readyRead(line_out);
					}
				});
			process.wait();
			th1.join();
			th2.join();
			mstate = Finsished;
			exit = process.exit_code();
			m_finished(exit);
			if (manager != nullptr)
				manager->finished();
			emit finished(exit);
			terminate();
		});
}

/**
 * @brief Rclone::waitForFinished, attend que le processus m_rclone se termine
 */
void Rclone::waitForFinished()
{
	if (mstate not_eq Running)
		waitForStarted();
	if (mstate == Running and mthread->joinable())
		mthread->join();
}

/**
 * @brief destructeur
 */
Rclone::~Rclone()
{
	terminate();
}

/**
 * @brief Rclone::terminate, termine le processus m_rclone
 */
void Rclone::terminate()
{
	if (mstate == Running)
	{

		mthread->detach();
		cout << "process rclone kill" << endl;
		Iridium::Utility::KillThread(mthread->native_handle());
		if (mthread->joinable())
			mthread->join();
		mstate = Finsished;
	}
}

/**
 * @brief Rclone::getState, retourne l'état du processus m_rclone
 * @return
 */
Rclone::State Rclone::getState() const
{
	return mstate;
}

/**
 * @brief Rclone::waitForStarted, attend que le processus m_rclone démarre
 */
void Rclone::waitForStarted()
{
	while (mstate != Running)
	{
		std::unique_lock<std::mutex> lock(m);
		v.wait(lock);
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
	return exit;
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


/**
 * @brief RclonesManager::RclonesManager, constructeur
 * @param nbMaxProcess nombre de processus m_rclone maximum
 */
RclonesManager::RclonesManager(unsigned nbMaxProcess) : nbMaxProcess(nbMaxProcess)
{}

/**
 * @brief RclonesManager::RclonesManager, constructeur
 */
RclonesManager::RclonesManager() : nbMaxProcess(thread::hardware_concurrency())
{}

/**
 * @brief RclonesManager::get, retourne un pointeur vers un m_rclone
 * @return un pointeur vers un m_rclone
 */
RclonePtr RclonesManager::get()
{
	rcloneVector.push_back(make_shared<Rclone>(this));
	return rcloneVector.back();
}

/**
 * @brief RclonesManager::start, un processus m_rclone appel cette fonction pour notifier le manager qu'il démarre
 */
void RclonesManager::start()
{
	while (nb_rclones >= nbMaxProcess)
	{
		std::unique_lock<std::mutex> lock(mutex);
		conditionVariable.wait(lock);
	}
	nb_rclones++;
}

/**
 * @brief RclonesManager::m_finished, un processus m_rclone appel cette fonction lorsqu'il se termine, pour notifier le manager
 */
void RclonesManager::finished()
{
	nb_rclones--;
	conditionVariable.notify_one();
	if (nb_rclones == 0 and rcloneVector.size() > 1)
		allFinished();
}

/**
 * @brief RclonesManager::m_finished, un processus m_rclone appel cette fonction lorsqu'il se termine, pour notifier le manager
 */
void RclonesManager::release(RclonePtr rclone)
{
	rcloneVector.erase(std::remove(rcloneVector.begin(), rcloneVector.end(), rclone), rcloneVector.end());
}