//
// Created by Rahman on 05/04/2023.
//

#include "Settings.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/dll.hpp>
#include <fstream>
#include <QTranslator>
#include <iostream>
#include <Global.hpp>
#include <iridium/process.hpp>
#include <iridium/entities/remote.hpp>

#include "AddNewRemoteDialog.hpp"
#include "AddNewRemoteDialog.hpp"

using namespace std;
using namespace Iridium;
using namespace boost;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;
using namespace iridium::rclone;

QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
property_tree::ptree Settings::_settings, Settings::_default;
signals2::signal<void()> Settings::list_remote_changed;
const map<Settings::Node, string> Settings::_nodes = {
        {Settings::Node::All,          ""},
        {Settings::Node::General,      "general"},
        {Settings::Node::Rclone,       "rclone"},
        {Settings::Node::RclonePath,   "rclone.path"},
        {Settings::Node::Theme,        "theme"},
        {Settings::Node::MaxProcess,   "rclone.max_process"},
        {Settings::Node::LoadType,     "rclone.load_type"},
        {Settings::Node::DirIconColor, "theme.dir_color"},
        {Settings::Node::Remotes,      "remotes"},
        {Settings::Node::MaxDepth,     "rclone.max_depth"},
        {Settings::Node::Flags,        "rclone.flags"},
        {Settings::Node::Language,     "general.language"},
        {Settings::Node::Width,        "general.size.width"},
        {Settings::Node::Height,       "general.size.height"},
        {Settings::Node::ReloadTime,   "general.reload_time"},
};
RcloneUniquePtr Settings::_rclone = Rclone::create_unique();

/**
 * @brief change the color of the directory icon
 * @param color
 */
void Settings::changeDirIcon(const Settings::ThemeColor &color)
{
    const QIcon YELLOW_DIR_ICO = QIcon::fromTheme("yellow-folder");
    const QIcon DEFAULT_DIR_ICO = QIcon::fromTheme("default-folder");
    const QIcon GREEN_DIR_ICO = QIcon::fromTheme("green-folder");
    const QIcon RED_DIR_ICO = QIcon::fromTheme("red-folder");
    const QIcon PURPLE_DIR_ICO = QIcon::fromTheme("purple-folder");
    const QIcon ORANGE_DIR_ICO = QIcon::fromTheme("orange-folder");
    const QIcon GRAY_DIR_ICO = QIcon::fromTheme("grey-folder");
    const QIcon PINK_DIR_ICO = QIcon::fromTheme("pink-folder");

    switch (color)
    {
        case Settings::ThemeColor::Yellow:
            DIR_ICON = YELLOW_DIR_ICO;
            break;
        case Settings::ThemeColor::Green:
            DIR_ICON = GREEN_DIR_ICO;
            break;
        case Settings::ThemeColor::Red:
            DIR_ICON = RED_DIR_ICO;
            break;
        case Settings::ThemeColor::Purple:
            DIR_ICON = PURPLE_DIR_ICO;
            break;
        case Settings::ThemeColor::Orange:
            DIR_ICON = ORANGE_DIR_ICO;
            break;
        case Settings::ThemeColor::Gray:
            DIR_ICON = GRAY_DIR_ICO;
            break;
        case Settings::ThemeColor::Pink:
            DIR_ICON = PINK_DIR_ICO;
            break;
        case Settings::ThemeColor::Default:
            DIR_ICON = DEFAULT_DIR_ICO;
            break;
    }
    setValue(Node::DirIconColor, static_cast<int>(color));
}

/**
 * @brief get list of local remotes
 * @return list of local remotes
 */
std::vector<RemoteInfoPtr> Settings::getLocalRemotes()
{
    std::vector<RemoteInfoPtr> remotes;
    try { auto remote_ptree = _settings.get_child(_nodes.at(Remotes)); }
    catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e)
    {
        cout << e.what() << endl;
        return remotes;
    }


    for (const auto &remote: _settings.get_child(_nodes.at(Remotes)))
    {
        try
        {
            remotes.emplace_back(std::make_shared<RemoteInfo>(
                    remote.second.get<string>("path"), entity::remote::none, remote.first));
        }
        catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e) { continue; }
    }
    return remotes;
}

void Settings::refreshRemotesList()
{
    std::vector<RemoteInfoPtr> distants;
    iridium::rclone::process().list_remotes([&distants](const std::vector<remote_ptr>& remotes)
    {
        for (auto &remote: remotes)
            distants.emplace_back(std::make_shared<RemoteInfo>(remote->name(), remote->type(), remote->full_path()));
    }).execute().wait_for_finish();
    auto locals = Settings::getLocalRemotes();
//    insert remotes in refresh
    distants.insert(distants.begin(), locals.begin(), locals.end());
    const auto remotes = &Iridium::Global::remotes;

    remotes->erase(std::ranges::remove_if(*remotes, [](const RemoteInfoPtr &remote)
    { return remote == nullptr; }).begin(), remotes->end());

    // remove remotes in remotes but not in distants
    for (const auto remote : *remotes)
    {
        if(std::ranges::none_of(distants.begin(), distants.end(), [remote](const RemoteInfoPtr &r)
        { return *r == *remote; }))
            remotes->erase(std::ranges::find_if(remotes->begin(), remotes->end(), [remote](const RemoteInfoPtr &r)
            { return *r == *remote; }));
    }
    // add remotes in distants but not in remotes
    for (const auto &remote: distants)
    {
        if (std::ranges::none_of(remotes->begin(), remotes->end(), [remote](const RemoteInfoPtr &r)
        { return *r == *remote; }))
            remotes->emplace_back(remote);
    }

    list_remote_changed();
}


/**
 * @brief add a local remote and save it
 * @param remoteInfo
 */
void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
    pt::ptree remote, ptree_path;
    ptree_path.put("", remoteInfo.full_path());
    remote.add_child("path", ptree_path);

    pt::ptree array = _settings.get_child(_nodes.at(Remotes));
    array.push_back(std::make_pair(remoteInfo.name(), remote));
    _settings.put_child(_nodes.at(Remotes), array);

    saveSettings();
}

/**
 * @brief delete a local remote
 * @param remoteInfo
 */
void Settings::deleteRemote(const RemoteInfoPtr &remoteInfo)
{
    pt::ptree array = _settings.get_child(_nodes.at(Remotes));
    for (auto it = array.begin(); it != array.end(); ++it)
    {
        if (it->first == remoteInfo->name())
        {
            array.erase(it);
            break;
        }
    }
    _settings.put_child(_nodes.at(Remotes), array);
    saveSettings();
}

/**
 * @brief init all app settings
 */
void Settings::init()
{
    // init rclone path
    iridium::rclone::process::initialize();
    auto rclonePath = boost::process::search_path("rclone");
    if (rclonePath.empty())
    {
        rclonePath = dll::program_location().parent_path().append("rclone");
        if (QSysInfo::productType() == "windows")
            rclonePath += ".exe";
    }
    Global::path_rclone = rclonePath.string();

    Settings::HARDDRIVE_ICON = QIcon::fromTheme("drive-harddisk-solidstate");
    initSettings();
    loadSettings();
    Settings::changeDirIcon(static_cast<ThemeColor>(getValue<uint8_t>(DirIconColor)));


    boost::filesystem::path translation_dir;
    if (getSystem().os == Os::MacOs)
    {
        translation_dir = dll::program_location().parent_path().parent_path().append("Translations");
    } else
        translation_dir = dll::program_location().parent_path().append("Translations");

    auto *translator = new QTranslator();
    auto ok = translator->load(QLocale(getValue<string>(Language).c_str()), "iridium", "_",
                               translation_dir.string().c_str());
    QApplication::installTranslator(translator);
    QLocale::setDefault(QLocale(getValue<string>(Language).c_str()));

}

/**
 * @brief load all app settings
 */
void Settings::loadSettings()
{
    auto path = Settings::getPathSettings();
    if (bf::exists(path))
    {
        ifstream ifs(path.string());
        string data((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());
        pt::read_json(getPathSettings().string(), _settings);
        loadValues();
    } else
    {
        bf::create_directories(path.parent_path());
        ofstream(path.string(), ios::out | ios::trunc).close();
        bf::permissions(path, bf::perms::owner_all);
        resetSettings(All);
    }

}

/**
 * @brief save all app settings
 */
void Settings::saveSettings()
{
    // overwrite settings file
    pt::write_json(getPathSettings().string(), _settings);
}

/**
 * @brief get path to settings file on current OS
 * @return path to settings file
 */
boost::filesystem::path Settings::getPathSettings()
{
    bf::path path;
    if (getSystem().os == Os::MacOs)
    {
        path = dll::program_location().parent_path().parent_path().append("Resources");
        path /= "settings.iridium";
    } else
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
        path = settings.fileName().toStdString();
        path = path.parent_path();
        path += "/settings.iridium";
    }
    return path;
}


void Settings::initSettings()
{
    _settings.put(_nodes.at(Language), QLocale::system().name().toStdString());
    _settings.put(_nodes.at(DirIconColor), 0);
    _settings.put(_nodes.at(LoadType), 0);
    _settings.put(_nodes.at(MaxDepth), 2);
    _settings.put(_nodes.at(RclonePath), Global::path_rclone);
    _settings.put(_nodes.at(MaxProcess), std::thread::hardware_concurrency());
    _settings.put(_nodes.at(Remotes), "");
    _settings.put(_nodes.at(Flags), "");
    _settings.put(_nodes.at(Width), 1200);
    _settings.put(_nodes.at(Height), 600);
    _settings.put(_nodes.at(ReloadTime), 10);

    pt::ptree remote, ptree_path;
    RemoteInfo remoteInfo = {"/", entity::remote::none, "Local"};
    ptree_path.put("", remoteInfo.full_path());
    remote.add_child("path", ptree_path);

    pt::ptree array = _settings.get_child(_nodes.at(Remotes));
    array.push_back(std::make_pair(remoteInfo.name(), remote));
    _settings.put_child(_nodes.at(Remotes), array);

    // init flags
    pt::ptree flags;
    for (int i = 0; i < 100; ++i)
    {
        if (Rclone::getFlag(static_cast<Rclone::Flag>(i)).value.empty())
            break;
        pt::ptree flag;
        flag.put("name", Rclone::getFlag(static_cast<Rclone::Flag>(i)).name);
        flag.put("value", Rclone::getFlag(static_cast<Rclone::Flag>(i)).value);
        flags.push_back(std::make_pair(std::to_string(i), flag));
    }
    _settings.put_child(_nodes.at(Flags), flags);

    _default = _settings;

//    std::stringstream ss;
//    property_tree::json_parser::write_json(ss, m_settings);
//    cout << ss.str() << endl;

}

void Settings::resetSettings(const Node &node)
{
    if (node == All)
    {
        _settings.clear();
        _settings = _default;
    } else
    {
        _settings.erase(_nodes.at(node));
        _settings.put_child(_nodes.at(node), _default.get_child(_nodes.at(node)));
    }
    saveSettings();
    loadValues();
}

/**
 * @brief load variables from _settings ptree
 */
void Settings::loadValues()
{
    //check if all nodes are present
    for (const auto &node: _nodes)
    {
        if (!_settings.get_child_optional(node.second))
        {
            resetSettings(node.first);
            break;
        }
    }
    try
    {
        auto flags = _settings.get_child(_nodes.at(Flags));
        for (const auto &flag: flags)
        {
            Rclone::setFlag(static_cast<Rclone::Flag>(std::stoi(flag.first)),
                            flag.second.get<string>("value"));
        }
        Global::load_type = static_cast<Iridium::Load>(getValue<uint8_t>(LoadType));
        Global::max_depth = getValue<uint8_t>(MaxDepth);
        Global::max_process = getValue<uint8_t>(MaxProcess);
        if (getValue<string>(RclonePath).contains("rclone"))
            Global::path_rclone = getValue<string>(RclonePath);
        Settings::changeDirIcon(static_cast<Settings::ThemeColor>(getValue<uint8_t>(DirIconColor)));
        Global::reload_time = getValue<uint8_t>(ReloadTime);

    } catch (boost::exception &e)
    {
        cout << diagnostic_information_what(e, true) << endl;
    }
}

void Settings::setRcloneFlag(const Rclone::Flag &flag, const std::string &value)
{
    pt::ptree flags;
    flags.put("name", Rclone::getFlag(flag).name);
    flags.put("value", value);

    pt::ptree array = _settings.get_child(_nodes.at(Flags));

    while (array.find(to_string(flag)) != array.not_found())
        array.erase(to_string(flag));
    array.push_back(std::make_pair(to_string(flag), flags));
    _settings.put_child(_nodes.at(Flags), array);

    Rclone::setFlag(flag, value);
    saveSettings();
}

std::string Settings::getRcloneFlag(const Rclone::Flag &flag)
{
    pt::ptree array = _settings.get_child(_nodes.at(Flags));
    for (auto &it: array)
    {
        if (it.first == to_string(flag))
            return it.second.get<string>("value");
    }
    resetSettings(All);
    return getRcloneFlag(flag);
}

void Settings::setLanguage(const QLocale::Language &lang)
{
    _settings.put(_nodes.at(Language), QLocale(lang).name().toStdString());
    saveSettings();
}

Settings::System Settings::getSystem()
{
    System system{};
#ifdef _WIN32
    system.os = Os::Windows;
#elif __APPLE__
    system.os = Os::MacOs;
#elif __linux__
    system.os = Os::Linux;
#endif

#ifdef __x86_64__
    system.arch = Arch::x86_64;
#elif __i386__
    system.arch = Arch::x86;
#elif __aarch64__
    system.arch = Arch::Arm64;
#endif
    return system;
}
