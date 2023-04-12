//
// Created by sr-71 on 09/01/2023.
//


#ifndef IRIDIUM_RCLONE_HPP
#define IRIDIUM_RCLONE_HPP

#ifndef __kernel_entry
#define __kernel_entry
#endif

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <boost/signals2.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/json.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <memory>
#include "RcloneFile.hpp"
#include <Remote.h>

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
	std::shared_ptr<boost::thread> mthread{};
	std::vector<std::string> m_out{};
	std::vector<std::string> m_error{};
	std::map<std::string, std::string> m_mapData{};
	uint8_t exit{};
	Rclone::State mstate{};
	RclonesManager *manager{};

	std::mutex m;
	std::condition_variable v;

public:

	[[nodiscard]] static const std::string &getPathRclone();

	static void setPathRclone(const std::string &pathRclone);

	void config(RemoteType type, const std::string &name, const std::vector<std::string> &args = {});

	void lsJson(const std::string &path);

	void copyTo(const RcloneFile &src, const RcloneFile &dest);

	void deleteRemote(const std::string &remote);

	void deleteFile(const RcloneFile &file);

	void mkdir(const RcloneFile &dir);

	void moveto(const RcloneFile &src, const RcloneFile &dest);

	void listRemotes();

	void size(const std::string &path);

	void waitForFinished();

	void terminate();

	[[nodiscard]] State getState() const;

	void waitForStarted();

	[[nodiscard]] std::map<std::string, std::string> getData() const;

	[[nodiscard]] std::vector<std::string> readAllError() const;

	[[nodiscard]] uint8_t exitCode() const;


private:
	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const std::string &)> m_readyRead{};
	boost::signals2::signal<void(const int exit)> m_finished{};

signals:

	void finished(int exit);

	void lsJsonFinished(const QJsonDocument &);

	void taskProgress(const boost::json::object &);

	void sizeFinished(const uint32_t &objs, const uint64_t &size, const std::string &strSize);

	void fileDeleted(const int &exit);


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
