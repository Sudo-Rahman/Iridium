//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTE_H
#define IRIDIUM_REMOTE_H

#include <map>
#include <iostream>


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

enum RemoteType
{
    LocalHardDrive,Drive, Sftp, OneDrive, Dropbox, Ftp, Mega,
    OpenDrive, Pcloud, Box, Smb,Cmd, END
};

const std::map<RemoteType, std::string> remoteIco = {
        {RemoteType::Drive,          DRIVEICON},
        {RemoteType::Sftp,           SFTPICON},
        {RemoteType::LocalHardDrive, HARDDRIVEICON},
        {RemoteType::OneDrive,       ONEDRIVEICON},
        {RemoteType::Dropbox,        DROPBOX},
        {RemoteType::Ftp,            FTP},
        {RemoteType::Mega,           MEGA},
        {RemoteType::OpenDrive,      OPENDRIVE},
        {RemoteType::Pcloud,         PCLOUD},
        {RemoteType::Box,            BOX},
        {RemoteType::Smb,            SMB},
        {RemoteType::Cmd,            CMD}

};
const std::map<std::string, RemoteType> stringToRemoteType = {
        {"drive",     RemoteType::Drive},
        {"sftp",      RemoteType::Sftp},
        {"local",     RemoteType::LocalHardDrive},
        {"onedrive",  RemoteType::OneDrive},
        {"dropbox",   RemoteType::Dropbox},
        {"ftp",       RemoteType::Ftp},
        {"mega",      RemoteType::Mega},
        {"opendrive", RemoteType::OpenDrive},
        {"pcloud",    RemoteType::Pcloud},
        {"box",       RemoteType::Box},
        {"smb",       RemoteType::Smb},
};

struct RemoteInfo
{
private:
    std::string _name;
public:
    RemoteType type;
    std::string icon;
    std::string path;

    auto setName(std::string name) { _name = std::move(name); }

    RemoteInfo(std::string path, RemoteType type, std::string name = "")
            : _name(std::move(name)), type(type), path(std::move(path))
    {
        icon = [&]() -> std::string
        {
            try
            {
                return remoteIco.at(type);
            } catch (std::out_of_range &e)
            {
                if(type == RemoteType::LocalHardDrive)
                    return HARDDRIVEICON;
                else
                    return CMD;
            }
        }();
        _name = [this]() -> auto
        {
            if (!_name.empty())
                return _name;
            if (isLocal())
                return RemoteInfo::path;
            else
            {
                if (RemoteInfo::path.find(':') != std::string::npos)
                    return RemoteInfo::path.substr(0, RemoteInfo::path.find(':'));
                else
                    return RemoteInfo::path;
            }
        }();

        RemoteInfo::path = [this]() -> auto
        {
            if (isLocal())
                return RemoteInfo::path;
            else
            {
                if (RemoteInfo::path.find(':') == std::string::npos)
                    return RemoteInfo::path + ":";
                else
                    return RemoteInfo::path;
            }
        }();
    }

    RemoteInfo() = default;

    [[nodiscard]] static const RemoteType findType(const std::string &type){
        try {
            return stringToRemoteType.at(type);
        } catch (std::out_of_range &e) {
            return RemoteType::Cmd;
        }
    }

    [[nodiscard]] bool isLocal() const { return type == RemoteType::LocalHardDrive; }

    [[nodiscard]] std::string name() const { return _name; }

    // overload operator == to compare two RemoteInfo
    bool operator==(const RemoteInfo &other) const
    {
        return _name == other._name && type == other.type && path == other.path;
    }

    bool operator==(const RemoteInfo *other) const { return *this == *other; }

    bool operator!=(const RemoteInfo &other) const { return !(*this == other); }

    bool operator!=(const RemoteInfo *other) const { return !(*this == other); }

    // overload operator << to print RemoteInfo
    friend std::ostream &operator<<(std::ostream &os, const RemoteInfo &info)
    {
        os << "RemoteInfo{"
           << "name=" << info._name << ", "
           << "type=" << info.type << ", "
           << "path=" << info.path << "}";
        return os;
    }
};

typedef std::shared_ptr<RemoteInfo> RemoteInfoPtr;

#endif //IRIDIUM_REMOTE_H
