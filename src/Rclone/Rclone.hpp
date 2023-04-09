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

	static std::shared_ptr<Rclone> instance(RclonesManager * = nullptr)
	{ return std::make_shared<Rclone>(); };

	~Rclone() override;

	enum State
	{
		NotLaunched, Running, Finsished
	};


private:
	static std::string m_pathRclone;
	std::shared_ptr<std::thread> mthread{};
	std::string mdata{};
	std::string m_error{};
	std::map<std::string, std::string> m_mapData{};
	pid_t pid{};
	Rclone::State mstate{};
	RclonesManager *manager{};

	std::mutex m;
	std::condition_variable v;

public:

	[[nodiscard]] static const std::string &getPathRclone() ;

	static void setPathRclone(const std::string &pathRclone);

	void config(RemoteType type, const std::string &name, const std::vector<std::string> &args = {});

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

	[[nodiscard]] std::map<std::string,std::string> getData() const;

	[[nodiscard]] std::string readAllError() const;


private:
	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const std::string &)> m_readyRead{};
	boost::signals2::signal<void(const int exit)> m_finished{};

signals:

	void finished(int exit);

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
