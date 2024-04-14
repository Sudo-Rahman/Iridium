//
// Created by rahman on 13/01/23.
//

#pragma once
#include <map>
#include <iridium/entities.hpp>


const std::string DRIVEICON = ":/ressources/google-drive.png";
const std::string SFTPICON = ":/ressources/sftp.png";
const std::string ONEDRIVEICON = ":/ressources/one-drive.png";
const std::string DROPBOX = ":/ressources/dropbox.png";
const std::string FTP = ":/ressources/ftp.png";
const std::string MEGA = ":/ressources/mega.png";
const std::string OPENDRIVE = ":/ressources/open-drive.png";
const std::string PCLOUD = ":/ressources/pcloud.png";
const std::string BOX = ":/ressources/box.png";
const std::string SMB = ":/ressources/smb.png";
const std::string CMD = ":/ressources/cmd.png";

const std::string HARDDRIVEICON = ":/ressources/drive-harddisk-solidstate.svg";


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
				{ire::remote::cmd, CMD}

		};


class RemoteInfo : public ire::remote
{
public:
	RemoteInfo(const std::string &name,remote_type type,const std::string& path) : remote(name, type, path)
	{}

	auto icon() const -> std::string { return remoteIco.at(type()); }

	[[nodiscard]] bool isLocal() const { return type() == none; }

	[[nodiscard]] std::string full_path() const override
	{
		if (isLocal())
			return path();
		return root_path() + path();
	}
};

typedef std::shared_ptr<RemoteInfo> RemoteInfoPtr;
