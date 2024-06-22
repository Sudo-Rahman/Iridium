//
// Created by rahman on 13/01/23.
//

#pragma once
#include <map>
#include <iridium/entities.hpp>

const std::string DRIVEICON = ":/resources/google-drive.png";
const std::string SFTPICON = ":/resources/sftp.png";
const std::string ONEDRIVEICON = ":/resources/one-drive.png";
const std::string DROPBOX = ":/resources/dropbox.png";
const std::string FTP = ":/resources/ftp.png";
const std::string MEGA = ":/resources/mega.png";
const std::string OPENDRIVE = ":/resources/open-drive.png";
const std::string PCLOUD = ":/resources/pcloud.png";
const std::string BOX = ":/resources/box.png";
const std::string SMB = ":/resources/smb.png";
const std::string CRYPT = ":/resources/crypt.png";
const std::string ALIAS = ":/resources/alias.png";
const std::string CMD = ":/resources/cmd.png";

const std::string HARDDRIVEICON = ":/resources/drive-harddisk-solidstate.svg";

const std::map<ire::remote::remote_type, std::string> remoteIco = {
				{ire::remote::google_drive, DRIVEICON},
				{ire::remote::sftp, SFTPICON},
				{ire::remote::none, HARDDRIVEICON},
				{ire::remote::onedrive, ONEDRIVEICON},
				{ire::remote::dropbox, DROPBOX},
				{ire::remote::ftp, FTP},
				{ire::remote::mega, MEGA},
				{ire::remote::opendrive, OPENDRIVE},
				{ire::remote::pcloud, PCLOUD},
				{ire::remote::box, BOX},
				{ire::remote::smb, SMB},
				{ire::remote::crypt, CRYPT},
				{ire::remote::alias, ALIAS},
				{ire::remote::cmd, CMD},
				{ire::remote::other, CMD},
		};

class RemoteInfo : public ire::remote
{
public:
	RemoteInfo(const std::string &name, remote_type type, const std::string &path) : remote(name, type, path) {}

	auto icon() const -> std::string
	{
		if (remoteIco.contains(type()))
			return remoteIco.at(type());
		return CMD;
	}

	auto getIcon() const -> QIcon { return QIcon(icon().c_str()); }

	[[nodiscard]] bool isLocal() const { return type() == none; }

	[[nodiscard]] std::string full_path() const override
	{
		if (isLocal())
			return path();
		return root_path() + path();
	}
};

typedef std::shared_ptr<RemoteInfo> RemoteInfoPtr;
