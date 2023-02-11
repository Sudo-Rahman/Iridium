//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <iostream>
#include <QCoreApplication>

namespace bp = boost::process;
using namespace std;


Rclone::Rclone(string path, RclonesManager *parent) : pathRclone(std::move(path)), manager(parent)
{
}

Rclone::Rclone(RclonesManager *parent) : pathRclone(
	qApp->applicationDirPath().append("/rclone").toStdString()), manager(parent)
{}

const string &Rclone::getPathRclone() const
{
	return pathRclone;
}

void Rclone::setPathRclone(const string &pathRclone)
{
	Rclone::pathRclone = pathRclone;
}


/**
 * @brief Liste sous forme de json les fichiers dans le dossier path
 * @param path
 */
void Rclone::lsJson(const string &path)
{
	finished.connect(
		[=](const int exit)
		{
			if (exit == 0)
			{
				auto doc = QJsonDocument::fromJson(
					QString::fromStdString(mdata).toUtf8());
				emit lsJsonFinished(doc);
			}
		});
	execute({"lsjson", path});
}

/**
 * @brief upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::copyTo(const RcloneFile &src, const RcloneFile &dest)
{
	vector<string> arguments(
		{"copyto", src.getPath().toStdString(), dest.getPath().toStdString() + src.getName().toStdString(), "-P"});

	readyRead.connect(
		[=](const string &data)
		{
			auto qdata = QString::fromStdString(data).split("\n");
			if (not qdata.isEmpty())
			{
				auto l1 = qdata[0].split(QRegularExpression(" |,"));
				erase_if(l1, [](const auto &item)
				{ return item == "" or item == "\t" or item == ","; });
				if (l1.size() > 11)
					emit copyProgress(l1[11].remove("%").toInt());
			}
		});
	execute(arguments);
	finished.connect([=](int ex)
					 { emit copyProgress(100); });

}


void Rclone::config(RemoteType type, const vector<string> &params)
{
	if (mstate == Running)
		terminate();
	if (params.size() < 0)
		return;
	finished.connect([=](int exit)
					 {
						 emit
							 configFinished(exit);
					 });
	switch (type)
	{

		case Drive:
			execute({"config", "create", params[0], "drive"});
			break;
		case Sftp:
			break;
	}
}

void Rclone::listRemotes()
{
	qDebug() << "efefz";
	finished.connect(
		[=](const int exit)
		{
			if (exit == 0)
			{
				auto data = QString::fromStdString(mdata).split("\n");
				erase_if(data, [](auto &str)
				{ return str == ""; });
				QMap<QString, QString> map;
				for (auto &str: data)
				{
					str.remove(" ");
					map.insert(str.split(":")[0], str.split(":")[1]);
				}
				listRemotesFinished(map);
			}
		});
	execute({"listremotes", "--long"});
}

void Rclone::deleteRemote(const string &remote)
{
	execute({"config", "delete", remote});
}

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
			 bp::ipstream ip;
			 auto process = bp::child(pathRclone, bp::args(args), bp::std_out > ip,
									  bp::std_err > stderr);
			 mstate = Running;

			 v.notify_one();

			 pid = process.id();
			 string line;
			 while (getline(ip, line))
			 {
				 mdata += line + "\n";
				 readyRead(line);
			 }
			 process.wait();
			 mstate = Finsished;
			 finished(process.exit_code());
			 if (manager != nullptr)
				 manager->finished();
		 });
}


void Rclone::waitForFinished()
{
	if (mstate not_eq Running)
		waitForStarted();
	if (mstate == Running and mthread->joinable())
		mthread->join();
}

Rclone::~Rclone()
{
	terminate();
	if (mstate == Running and mthread->joinable())
	{
		mthread->join();
		qDebug() << this;
	}
}

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

Rclone::State Rclone::getState() const
{
	return mstate;
}

void Rclone::waitForStarted()
{
	while (mstate != Running)
	{
		std::unique_lock<std::mutex> lock(m);
		v.wait(lock);
	}
}

void Rclone::size(const string &path)
{
	finished.connect(
		[=](const int exit)
		{
			if (exit == 0)
			{
				auto data = QString::fromStdString(mdata).split("\n");
				if (data.size() > 1)
				{
					auto l1 = QRegularExpression("\\(\\d+\\)").match(data[0]).captured(0).remove("(").remove(")").toUInt();
					auto l2 = QRegularExpression("\\d+.\\d+ \\w+").match(data[1]).captured(0);
					auto l3 = QRegularExpression("\\(\\d+").match(data[1]).captured(0).remove("(").toULongLong();
					emit sizeFinished(l1, l3, l2);
				}
			}
		});
	execute({"size", path});

}

RclonesManager::RclonesManager(unsigned nbMaxProcess) : nbMaxProcess(nbMaxProcess)
{}

RclonesManager::RclonesManager() : nbMaxProcess(thread::hardware_concurrency())
{}

std::shared_ptr<Rclone> RclonesManager::get()
{
	rcloneVector.push_back(make_shared<Rclone>(this));
	return rcloneVector.back();
}

void RclonesManager::start()
{
	while (nb_rclones >= nbMaxProcess)
	{
		std::unique_lock<std::mutex> lock(mutex);
		conditionVariable.wait(lock);
	}
	nb_rclones++;
}

void RclonesManager::finished()
{
	nb_rclones--;
	conditionVariable.notify_one();
	if (nb_rclones == 0 and rcloneVector.size() > 1)
		allFinished();
}


RclonesManager::~RclonesManager()
{
}
