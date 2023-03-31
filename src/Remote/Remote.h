//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTE_H
#define IRIDIUM_REMOTE_H

#include <map>

const std::string DRIVEICON = ":/ressources/google_drive.png";
const std::string SFTPICON = ":/ressources/sftp.png";
const std::string HARDDRIVEICON = "drive-harddisk-solidstate";

enum Remote
{
	Local,Distant
};

enum RemoteType
{
	Drive, Sftp, LocalHardDrive
};

const std::map<RemoteType, std::string> remoteIco = {
	{RemoteType::Drive,          DRIVEICON},
	{RemoteType::Sftp,           SFTPICON},
	{RemoteType::LocalHardDrive, HARDDRIVEICON}
};
const std::map<std::string, RemoteType> stringToRemoteType = {
	{"drive", RemoteType::Drive},
	{"sftp",  RemoteType::Sftp},
	{"local", RemoteType::LocalHardDrive}
};

struct RemoteInfo
{
private:
	std::string m_name;
public:
	RemoteType m_type;
	std::string m_icon;
	std::string m_path;

	RemoteInfo(std::string name, RemoteType type, std::string path = "")
		: m_name(std::move(name)), m_type(type), m_path(std::move(path))
	{
		m_icon = [&]() -> std::string
		{
			try
			{
				return remoteIco.at(type);
			} catch (std::out_of_range &e)
			{
				return HARDDRIVEICON;
			}
		}();
		m_path = [=]() -> auto
		{
			if (isLocal())
				return m_path;
			else
			{
				if (m_path.find(':') == std::string::npos)
					return m_path + ":";
				else
					return m_path;
			}
		}();
	}

	RemoteInfo() = default;
	[[nodiscard]] bool isLocal() const
	{
		return m_type == RemoteType::LocalHardDrive;
	}

	[[nodiscard]] std::string name() const
	{
		if (isLocal())
			return m_name;
		else
		{
			return [=]() -> auto
			{
				if (m_name.find(':') == std::string::npos)
					return m_name + ":";
				else
					return m_name;
			}();
		}
	}
};

//typedef std::shared_ptr<RemoteInfo> RemoteInfoPtr;


auto toQString = [](const std::string &str) -> QString
{
	return QString::fromStdString(str);
};

auto toString = [](const QString &str) -> std::string
{
	return str.toStdString();
};

#endif //IRIDIUM_REMOTE_H
