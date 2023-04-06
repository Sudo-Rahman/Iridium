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
#include "../Remote/Remote.h"

class RclonesManager;

class Rclone : public QObject
{
Q_OBJECT
public:
	explicit Rclone(std::string path, RclonesManager * = nullptr);

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
	std::map<std::string, std::string> m_mapData{};
	pid_t pid{};
	Rclone::State mstate{};
	RclonesManager *manager{};

	std::mutex m;
	std::condition_variable v;

public:

	[[nodiscard]] const std::string &getPathRclone() const;

	void setPathRclone(const std::string &pathRclone);

	void config(RemoteType type, const std::vector<std::string> &args);

	void lsJson(const std::string &path);

	void copyTo(const RcloneFile &src, const RcloneFile &dest);

	void deleteRemote(const std::string &remote);

	void deleteFile(const RcloneFile &file);

	void listRemotes();

	void size(const std::string &path);

	void waitForFinished();

	void terminate();

	[[nodiscard]] State getState() const;

	void waitForStarted();

	auto getData() const
	{
		return m_mapData;
	}


private:
	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const std::string &)> readyRead{};
public:
	boost::signals2::signal<void(const int exit)> finished{};
	boost::signals2::signal<void(const std::map<std::string, std::string>)> listRemotesFinished{};
	boost::signals2::signal<void(const QJsonDocument &)> lsJsonFinishedd{};


signals:

	void lsJsonFinished(const QJsonDocument &);

	void copyProgress(const int);

	void configFinished(int exit);

	void sizeFinished(uint32_t objs, uint64_t size, QString strSize);

	void fileDeleted(int exit);


};
typedef std::shared_ptr<Rclone> RclonePtr;

class RclonesManager
{
	friend class Rclone;

	std::atomic<unsigned> nb_rclones{};
	unsigned nbMaxProcess{};
	std::mutex mutex{};
	std::condition_variable conditionVariable{};
	std::vector<RclonePtr> rcloneVector{};

public:
	explicit RclonesManager(unsigned nbMaxProcess);

	explicit RclonesManager();

	RclonePtr get();

	void release(RclonePtr rclone);

	boost::signals2::signal<void()> allFinished;

private:
	void start();

	void finished();

};
typedef std::shared_ptr<RclonesManager> RclonesManagerPtr;

#endif //IRIDIUM_RCLONE_HPP
