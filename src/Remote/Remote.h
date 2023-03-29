//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTE_H
#define IRIDIUM_REMOTE_H

#include <map>


const std::string DRIVEICON = ":/ressources/google_drive.png";
const std::string SFTPICON = ":/ressources/sftp.png";
const std::string LOCALHARDDRIVEICON = "drive-harddisk-solidstate";


enum Remote
{
	Distant, Local
};

enum RemoteType
{
	Drive, Sftp, LocalHardDrive
};

const std::map<RemoteType, std::string> remoteIco = {
	{RemoteType::Drive, DRIVEICON},
	{RemoteType::Sftp,  SFTPICON}
};
const std::map<std::string, RemoteType> stringToRemoteType = {
	{"drive", RemoteType::Drive},
	{"sftp",  RemoteType::Sftp},
};
#endif //IRIDIUM_REMOTE_H
