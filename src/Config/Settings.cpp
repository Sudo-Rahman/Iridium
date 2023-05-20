//
// Created by Rahman on 05/04/2023.
//

#include "Settings.hpp"
#include <Rclone.hpp>
#include <RcloneFileModelDistant.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dll.hpp>
#include <fstream>
#include <QTranslator>
#include <memory>

using namespace std;
using namespace boost;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;

QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
string Rclone::_path_rclone;
property_tree::ptree Settings::_settings, Settings::_default;
signals2::signal<void()> Settings::list_remote_changed;
const map<Settings::Node, string> Settings::_nodes = {
        {Settings::Node::All,          ""},
        {Settings::Node::General,      "general"},
        {Settings::Node::Rclone,       "rclone"},
        {Settings::Node::Theme,        "theme"},
        {Settings::Node::MaxProcess,   "rclone.max_process"},
        {Settings::Node::LoadType,     "rclone.load_type"},
        {Settings::Node::DirIconColor, "theme.dir_color"},
        {Settings::Node::Remotes,      "remotes"},
        {Settings::Node::MaxDepth,     "rclone.max_depth"},
        {Settings::Node::Flags,        "rclone.flags"},
        {Settings::Node::Language,     "general.language"}
};

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
                    remote.second.get<string>("path"), RemoteType::LocalHardDrive, remote.first));
        }
        catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e) { continue; }
    }
    return remotes;
}

void Settings::refreshRemotesList()
{
    auto rclone = RcloneManager::get();
    rclone->listRemotes();
    rclone->waitForFinished();
    std::vector<RemoteInfoPtr> remotes;
    auto locales = getLocalRemotes();
    remotes.insert(remotes.end(), locales.begin(), locales.end());
    for (const auto &pair: rclone->getData())
        remotes.emplace_back(
                std::make_shared<RemoteInfo>(pair.first, stringToRemoteType.find(pair.second)->second));
    Iridium::Variable::remotes = remotes;
    list_remote_changed();
}


/**
 * @brief add a local remote and save it
 * @param remoteInfo
 */
void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
    pt::ptree remote, ptree_path;
    ptree_path.put("", remoteInfo.path);
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
    auto rclonePath = dll::program_location().parent_path().append("rclone");
    if (QSysInfo::productType() == "windows")
        rclonePath += ".exe";
    Rclone::setPathRclone(rclonePath.string());

    Settings::HARDDRIVE_ICON = QIcon::fromTheme("drive-harddisk-solidstate");
    initSettings();
    loadSettings();
    Settings::changeDirIcon(static_cast<ThemeColor>(getValue<uint8_t>(DirIconColor)));

    boost::filesystem::path translation_dir;
    if (QSysInfo::productType() == "macos")
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
        initValues();
    } else
    {
        bf::create_directories(path.parent_path());
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
    if (QSysInfo::productType() == "macos")
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
    _settings.put(_nodes.at(MaxProcess), std::thread::hardware_concurrency());
    _settings.put(_nodes.at(Remotes), "");
    _settings.put(_nodes.at(Flags), "");

    pt::ptree remote, ptree_path;
    RemoteInfo remoteInfo = {"/", RemoteType::LocalHardDrive, "Local"};
    ptree_path.put("", remoteInfo.path);
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
    initValues();
}

/**
 * @brief load variables from m_settings ptree
 */
void Settings::initValues()
{
    try
    {
        auto flags = _settings.get_child(_nodes.at(Flags));
        for (const auto &flag: flags)
        {
            Rclone::setFlag(static_cast<Rclone::Flag>(std::stoi(flag.first)),
                            flag.second.get<string>("value"));
        }
        RcloneFileModelDistant::setLoadType(
                static_cast<RcloneFileModelDistant::Load>(getValue<uint8_t>(LoadType)));
        RcloneFileModelDistant::setMaxDepth(getValue<uint8_t>(MaxDepth));
        RcloneManager::setMaxProcess(getValue<uint8_t>(MaxProcess));
        Settings::changeDirIcon(static_cast<Settings::ThemeColor>(getValue<uint8_t>(DirIconColor)));

    } catch (boost::exception &e)
    {
        cout << diagnostic_information_what(e, true) << endl;
    }
}

/**
 * @brief set value to node path in m_settings ptree
 * @param node
 * @param val
 */
void Settings::setValue(const Node &node, const auto &val)
{
    try
    {
        _settings.put(_nodes.at(node), val);
        saveSettings();
    } catch (boost::exception &e)
    {
        cout << "eror set Value" << diagnostic_information_what(e, true) << endl;
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
