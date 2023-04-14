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


class Rclone : public QObject
{
Q_OBJECT

public:
	explicit Rclone(std::string path);

	explicit Rclone();

	static std::shared_ptr<Rclone> instance()
	{ return std::make_shared<Rclone>(); };

	~Rclone() override;

	enum State
	{
		NotLaunched, Running, Finsished
	};

	enum TaskType
	{
		Unknown, Copy, Move, Delete, Mkdir, Rename
	};


private:

	boost::process::child m_child{};

	static std::string m_pathRclone;
	std::shared_ptr<boost::thread> m_thread{};
	std::vector<std::string> m_out{};
	std::vector<std::string> m_error{};
	std::map<std::string, std::string> m_mapData{};
	uint8_t m_exit{};
	Rclone::State m_state= Rclone::NotLaunched;

	std::mutex m_mutex;
	std::condition_variable m_cv;

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


	boost::signals2::signal<void(const std::string &)> m_readyRead{};
private:

	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const int exit)> m_finished{};

	static boost::json::object parseJson(const std::string &str);

	void connectTaskSignalFinishedJson();


signals:

	void finished(int exit);

	void lsJsonFinished(const QJsonDocument &);

	void taskProgress(const boost::json::object &);

	void sizeFinished(const uint32_t &objs, const uint64_t &size, const std::string &strSize);

	void taskFinished(const int &exit, const boost::json::object &);

};

typedef std::shared_ptr<Rclone> RclonePtr;

class RcloneManager
{

	friend class Rclone;

	static std::atomic_uint32_t m_nbRclone;
	static uint16_t m_nbMaxProcess;
	static std::mutex m_mutex;
	static std::condition_variable m_cv;
	static std::vector<RclonePtr> m_rcloneVector;

public:
	explicit RcloneManager(unsigned nbMaxProcess);

	explicit RcloneManager();

	static RclonePtr get();

	static void release(const RclonePtr& rclone);

private:
	static void start();

	static void finished();

};

typedef std::shared_ptr<RcloneManager> RcloneManagerPtr;

#endif //IRIDIUM_RCLONE_HPP
