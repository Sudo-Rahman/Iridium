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
#include "RcloneFile.hpp"
#include "../Remote/Remote.h"

class Rclone
{
public:
	explicit Rclone(std::string path);

	explicit Rclone();

	Rclone(const Rclone &rclone);

	~Rclone();

	enum State
	{
		NotLaunched, Running, Finsished
	};


private:
	std::string pathRclone{};
	std::thread *mthread{};
	std::string mdata{};
	pid_t pid{};
	Rclone::State mstate{};

	bool starte{};
	std::mutex m;
	std::condition_variable v;

public:

	[[nodiscard]] const std::string &getPathRclone() const;

	void setPathRclone(const std::string &pathRclone);

	void config(RemoteType type, const std::vector<std::string> &args);

	void lsJson(const std::string &path);

	void upload(const RcloneFile &src, const RcloneFile &dest);

	void download(const RcloneFile &src, const RcloneFile &dest);

	void deleteRemote(const std::string &remote);

	void listRemotes();

	void waitForFinished();

	void terminate();

	[[nodiscard]] State getState() const;

	void waitForStarted();

	Rclone &operator=(Rclone &&rclone) noexcept;


private:
	void execute(const std::vector<std::string> &args);

	boost::signals2::signal<void(const std::string &)> readyRead{};
public:
	boost::signals2::signal<void(const int exit)> finished{};
	boost::signals2::signal<void(const QMap<QString, QString>)> listRemotesFinished{};
	boost::signals2::signal<void(const QJsonDocument)> lsJsonFinished{};
	boost::signals2::signal<void(const double)> copyProgress{};


};


#endif //IRIDIUM_RCLONE_HPP
