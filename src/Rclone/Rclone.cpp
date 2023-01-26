//
// Created by sr-71 on 09/01/2023.
//

#include "Rclone.hpp"

#include <utility>
#include <iostream>
#include <QCoreApplication>
#include <boost/process.hpp>

namespace bp = boost::process;
using namespace std;


Rclone::Rclone(string path) : pathRclone(std::move(path))
{
}

Rclone::Rclone() : pathRclone(qApp->applicationDirPath().append("/rclone").toStdString())
{}

Rclone::Rclone(const Rclone &rclone)
{
	Rclone::pathRclone = rclone.getPathRclone();
}


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
	if (mstate == Running and mthread->joinable())
	{
		terminate();
		mthread->join();
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

