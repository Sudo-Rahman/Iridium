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

	mthread = new std::
	thread([=]
		   {
			   finished.connect([=](const int exit)
								{
									if (exit == 0)
									{
										auto doc = QJsonDocument::fromJson(
											QString::fromStdString(mdata).toUtf8());
										lsJsonFinished(doc);
									}
								});
			   execute({"lsjson", "--fast-list", "--drive-skip-gdocs", path});
		   });
}

/**
 * @brief upload d'un fichier local src vers la destination dest.
 * @param src
 * @param dest
 */
void Rclone::upload(const RcloneFile &src, const RcloneFile &dest)
{
	vector<string> arguments(
		{"copyto", src.getPath().toStdString(), dest.getPath().toStdString() + src.getName().toStdString(), "-P"});

	mthread = new std::
	thread([=]()
		   {
			   readyRead.connect([=](const string &data)
								 {
									 auto qdata = QString::fromStdString(data).split("\n");
									 if (not qdata.isEmpty())
									 {
										 auto l1 = qdata[0].split(QRegularExpression(" |,"));
										 erase_if(l1, [](const auto &item)
										 { return item == "" or item == "\t" or item == ","; });
										 if (l1.size() > 6)
											 copyProgress(l1[6].remove("%").toDouble());
									 }
								 });
			   execute(arguments);
			   copyProgress(100.0);
		   });

}

/**
 * @brief telecharge le fichier distant src vers la destination local dest.
 * @param src
 * @param dest
 */
void Rclone::download(const RcloneFile &src, const RcloneFile &dest)
{
	vector<string> arguments(
		{"copyto", src.getPath().toStdString(), dest.getPath().toStdString() + src.getName().toStdString(), "-P"});
	mthread = new std::thread([=]()
							  {
								  readyRead.connect([=](const string &data)
													{
														auto qdata = QString::fromStdString(data).split("\n");
														if (not qdata.isEmpty())
														{
															auto l1 = qdata[0].split(QRegularExpression(" |,"));
															erase_if(l1, [](const auto &item)
															{ return item == "" or item == "\t" or item == ","; });
															if (l1.size() > 6)
																copyProgress(l1[6].remove("%").toDouble());
														}
													});
								  execute(arguments);
								  copyProgress(100.0);
							  });
}

void Rclone::config(RemoteType type, const vector<string> &params)
{
	terminate();
	if (params.size() < 0)
		return;
	mstate = Running;
	mthread = new std::thread([=]()
							  {
								  bp::child process;
								  switch (type)
								  {
									  case Drive:
									  {
										  process = bp::child(pathRclone,
															  bp::args({"config", "create", params[0], "drive"}));
									  }


									  case Sftp:
										  break;
								  }
								  pid = process.id();
								  process.wait();
								  mstate = Finsished;
							  });

}

void Rclone::listRemotes()
{
	mthread = new std::
	thread([=]()
		   {
			   finished.connect([=](const int exit)
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
		   });
}

void Rclone::deleteRemote(const string &remote)
{
	mthread = new std::
	thread([=]()
		   {
			   execute({"config", "delete", remote});
		   });
}

void Rclone::execute(const vector<string> &args)
{
	std::unique_lock<std::mutex> lock(m);
	starte = true;
	v.notify_one();
	mdata.clear();
	terminate();
	mstate = Running;
	bp::ipstream ip;
	auto process = bp::child(pathRclone, bp::args(args), bp::std_out > ip, bp::std_err > stderr);
	pid = process.id();
	string line;
	while (getline(ip, line))
	{
		mdata += line + "\n";
		readyRead(line);
	}
	process.wait();
	finished(process.exit_code());
	mstate = Finsished;
}


Rclone &Rclone::operator=(Rclone &&rclone) noexcept
{
	if (this == &rclone)
		return *this;

	this->pathRclone = rclone.pathRclone;
	return *this;
}

void Rclone::waitForFinished()
{
	if (mstate not_eq Running)
		waitForStarted();
	if (mstate == Running)
		mthread->join();
}

Rclone::~Rclone()
{
	terminate();
	mthread->join();
	delete mthread;
}

void Rclone::terminate()
{
	cout << mstate << endl;
	if (mstate == Running)
	{
		kill(pid, SIGKILL);
		cout <<pid<< " kill" << endl;
	}
	mstate = Finsished;
}

Rclone::State Rclone::getState() const
{
	return mstate;
}

void Rclone::waitForStarted()
{
	std::unique_lock<std::mutex> lock(m);
	v.wait(lock, [&]
	{ return starte; });

}

