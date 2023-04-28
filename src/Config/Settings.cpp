//
// Created by sr-71 on 05/04/2023.
//

#include "Settings.hpp"
#include <Rclone.hpp>
#include <RcloneFileModelDistant.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dll.hpp>
#include <fstream>

using namespace std;
using namespace boost;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;

QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
string Rclone::m_pathRclone;
property_tree::ptree Settings::m_settings;
const map<Settings::Node, string> Settings::m_nodes = {
	{Settings::Node::MaxProcess,   "general.max_process"},
	{Settings::Node::LoadType,     "general.load_type"},
	{Settings::Node::DirIconColor, "theme.color"},
	{Settings::Node::Remotes,      "rclone.remotes"},
	{Settings::Node::MaxDepth,     "general.max_depth"},
	{Settings::Node::Flags,        "rclone.flags"}
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
QList<RemoteInfoPtr> Settings::getLocalRemotes()
{
	QList<RemoteInfoPtr> remotes;
	try
	{ auto remote_ptree = m_settings.get_child(m_nodes.at(Remotes)); }
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e)
	{
		cout << e.what() << endl;
		return remotes;
	}


	for (const auto &remote: m_settings.get_child(m_nodes.at(Remotes)))
	{
		try
		{
			remotes.append(std::make_shared<RemoteInfo>(
				remote.second.get<string>("path"), RemoteType::LocalHardDrive, remote.first));
		}
		catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e)
		{ continue; }
	}
	return remotes;
}

/**
 * @brief add a local remote and save it
 * @param remoteInfo
 */
void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
	pt::ptree remote, ptree_path;
	ptree_path.put("", remoteInfo.m_path);
	remote.add_child("path", ptree_path);

	pt::ptree array = m_settings.get_child(m_nodes.at(Remotes));
	array.push_back(std::make_pair(remoteInfo.name(), remote));
	m_settings.put_child(m_nodes.at(Remotes), array);

	saveSettings();
}

/**
 * @brief delete a local remote
 * @param remoteInfo
 */
void Settings::deleteRemote(const RemoteInfoPtr &remoteInfo)
{
	pt::ptree array = m_settings.get_child(m_nodes.at(Remotes));
	for (auto it = array.begin(); it != array.end(); ++it)
	{
		if (it->first == remoteInfo->name())
		{
			array.erase(it);
			break;
		}
	}
	m_settings.put_child(m_nodes.at(Remotes), array);
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
		pt::read_json(getPathSettings().string(), m_settings);
		initValues();
//		std::stringstream ss;
//		property_tree::json_parser::write_json(ss, m_settings);
//		cout << ss.str() << endl;
	} else
		resetSettings();

}

/**
 * @brief save all app settings
 */
void Settings::saveSettings()
{
	// overwrite settings file
	pt::write_json(getPathSettings().string(), m_settings);
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
		path = dll::program_location().parent_path();
		path /= "settings.iridium";
	}
	return path;
}


void Settings::initSettings()
{
	m_settings.put(m_nodes.at(DirIconColor), 0);
	m_settings.put(m_nodes.at(LoadType), 0);
	m_settings.put(m_nodes.at(MaxDepth), 2);
	m_settings.put(m_nodes.at(MaxProcess), std::thread::hardware_concurrency());
	m_settings.put(m_nodes.at(Remotes), "");

	pt::ptree arrayFlags, flag;
	flag.put("value", 4);
	arrayFlags.push_back(std::make_pair("--transfers", flag));

	m_settings.add_child(m_nodes.at(Flags), arrayFlags);

//	std::stringstream ss;
//	property_tree::json_parser::write_json(ss, m_settings);
//	cout << ss.str() << endl;

}

void Settings::resetSettings()
{
	m_settings.clear();
	initSettings();
	saveSettings();
}

/**
 * @brief load variables from m_settings ptree
 */
void Settings::initValues()
{
	try
	{
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
		m_settings.put(m_nodes.at(node), val);
		saveSettings();
	} catch (boost::exception &e)
	{
		cout << "eror set Value" << diagnostic_information_what(e, true) << endl;
	}
}

