//
// Created by sr-71 on 09/01/2023.
//


#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <boost/signals2.hpp>
#include <boost/process.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <memory>
#include "RcloneFile.hpp"
#include "RclonesManager.hpp"
#include "../Remote/Remote.h"

class Rclone : public QObject
{
Q_OBJECT
public:
	explicit Rclone(std::string path,RclonesManager * = nullptr);

	explicit Rclone(RclonesManager * = nullptr);

	~Rclone() override;

	enum State
	{
		NotLaunched, Running, Finsished
	};


private:
	std::string pathRclone{};
	std::shared_ptr<std::thread> mthread{};
	std::string mdata{};
	pid_t pid{};
	Rclone::State mstate{};
	RclonesManager * manager{};

	std::mutex m;
	std::condition_variable v;

public:

	[[nodiscard]] const std::string &getPathRclone() const;

	void setPathRclone(const std::string &pathRclone);

	void config(RemoteType type, const std::vector<std::string> &args);

	void lsJson(const std::string &path);

	void copyTo(const RcloneFile &src, const RcloneFile &dest);

	void deleteRemote(const std::string &remote);

	void listRemotes();

	void waitForFinished();

	void terminate();

	[[nodiscard]] State getState() const;

	void waitForStarted();


private:
	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const std::string &)> readyRead{};
public:
	boost::signals2::signal<void(const int exit)> finished{};
	boost::signals2::signal<void(const QMap<QString, QString>)> listRemotesFinished{};
	boost::signals2::signal<void(const QJsonDocument &)> lsJsonFinishedd{};


signals:

	void lsJsonFinished(const QJsonDocument &);

	void copyProgress(const int);

	void configFinished(int exit);


};


#endif //IRIDIUM_RCLONE_HPP
