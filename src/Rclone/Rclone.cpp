//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <iostream>

namespace bp = boost::process;
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
{
}

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
		[=](const int exit)
		{
			if (exit == 0)
			{
				auto doc = QJsonDocument::fromJson(
					QString::fromStdString(mdata).toUtf8());
				emit lsJsonFinished(doc);
			}
		});
	execute({"lsjson", path, "--drive-skip-gdocs"});
}

/**
 * @brief Upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::copyTo(const RcloneFile &src, const RcloneFile &dest)
{
	vector<string> arguments(
		{"copyto", src.getPath().toStdString(), dest.getPath().toStdString() + "/" + src.getName().toStdString(),
		 "-P"});

	m_readyRead.connect(
		[=](const string &data)
		{
			auto qdata = QString::fromStdString(data).split("\n");
			if (not qdata.isEmpty())
			{
				auto l1 = qdata[0].split(QRegularExpression(" |,"));
				erase_if(l1, [](const auto &item)
				{ return item == "" or item == "\t" or item == ","; });
				if (l1.size() > 11)
				{
					if (l1[11].remove("%").toInt() != 100)
						emit copyProgress(l1[11].remove("%").toInt());
				}
			}
		});
	execute(arguments);
	m_finished.connect([=](int ex)
					   { emit copyProgress(100); });
}


/**
 * @brief Rclone::deleteFile, supprime un fichier
 * @param file
 */
void Rclone::deleteFile(const RcloneFile &file)
{
	m_finished.connect(
		[=](const int exit)
		{ emit fileDeleted(exit); });
	if (!file.isDir())
		execute({"deletefile", file.getPath().toStdString(), "-v"});
	else
		execute({"purge", file.getPath().toStdString(), "-v"});
}


/**
 * @brief Rclone::config, permet de configurer un nouveau remote
 * @param type
 * @param params
 */
void Rclone::config(RemoteType type, const string &name, const vector<string> &params)
{
	if (mstate == Running)
		terminate();
	m_finished.connect([=](int exit)
					   {
						   emit
							   configFinished(exit);
					   });

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
 * @brief Rclone::listRemotes, permet de lister les remotes configurés
 */
void Rclone::listRemotes()
{
	m_finished.connect(
		[this](const int exit)
		{
			if (exit == 0)
			{
				auto data = QString::fromStdString(mdata).split("\n");
				erase_if(data, [](auto &str)
				{ return str == ""; });
				map<string, string> map;
				for (auto &str: data)
				{
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
	if (mstate == Running)
		terminate();
	mthread = std::make_shared<std::thread>
		([=]()
		 {
			 if (manager != nullptr)
				 manager->start();
			 mdata.clear();
			 bp::ipstream out;
			 bp::ipstream err;
			 auto process = bp::child(m_pathRclone, bp::args(args), bp::std_out > out,
									  bp::std_err > err);
			 mstate = Running;

			 v.notify_one();

			 pid = process.id();
			 string line;
			 while (getline(out, line))
			 {
				 mdata += line + "\n";
				 m_readyRead(line);
			 }
			 while (getline(err, line))
			 { m_error += line + "\n"; }
			 process.wait();
			 mstate = Finsished;
			 m_finished(process.exit_code());
			 if (manager != nullptr)
				 manager->finished();
			 emit finished(process.exit_code());
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
	if (mstate == Running and mthread->joinable())
	{
		mthread->join();
	}
}

/**
 * @brief Rclone::terminate, termine le processus m_rclone
 */
void Rclone::terminate()
{
	if (pid != 0)
	{
//		cout << pid << " kill" << endl;
		kill(pid, SIGKILL);
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
		[=](const int exit)
		{
			if (exit == 0)
			{
				auto data = QString::fromStdString(mdata).split("\n");
				if (data.size() > 1)
				{
					auto l1 = QRegularExpression("\\(\\d+\\)").match(data[0]).captured(0).remove("(").remove(
						")").toUInt();
					auto l2 = QRegularExpression("\\d+.\\d+ \\w+").match(data[1]).captured(0);
					auto l3 = QRegularExpression("\\(\\d+").match(data[1]).captured(0).remove("(").toULongLong();
					emit sizeFinished(l1, l3, l2);
				}
			}
		});
	execute({"size", path});

}

map<string, string> Rclone::getData() const
{
	return m_mapData;
}

string Rclone::readAllError() const
{
	return m_error;
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