//
// Created by sr-71 on 05/04/2023.
//

#include "Settings.hpp"
#include <Rclone.hpp>
#include <RcloneFileModelDistant.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

using namespace std;
using namespace boost;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;

QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
string Rclone::m_pathRclone;
property_tree::ptree Settings::m_settings;


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
	setValues("thme.color", static_cast<int>(color));
}

QList<RemoteInfoPtr> Settings::getLocalRemotes()
{
	QList<RemoteInfoPtr> remotes;
	try
	{ auto remote_ptree = m_settings.get_child("remotes"); }
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path> &e)
	{
		cout << e.what() << endl;
		return remotes;
	}


	for (const auto &remote: m_settings.get_child("remotes"))
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


void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
	pt::ptree remote, ptree_path;
	ptree_path.put("", remoteInfo.m_path);
	remote.add_child("path", ptree_path);

	pt::ptree array = m_settings.get_child("remotes");
	array.push_back(std::make_pair(remoteInfo.name(), remote));
	m_settings.put_child("remotes", array);

	saveSettings();
}

void Settings::deleteRemote(const RemoteInfoPtr &remoteInfo)
{
	auto *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
	settings->beginGroup("LocalRemotes");
	settings->remove(QString::fromStdString(remoteInfo->name()));
}


void Settings::init()
{

	auto rclonePath = bf::current_path().append("rclone");
	if (QSysInfo::productType() == "windows")
		rclonePath += ".exe";
	Rclone::setPathRclone(rclonePath.string());

	Settings::HARDDRIVE_ICON = QIcon::fromTheme("drive-harddisk-solidstate");
	initSettings();
	loadSettings();
	Settings::changeDirIcon(getDirIconColor());
}

Settings::ThemeColor Settings::getDirIconColor()
{
	return static_cast<Settings::ThemeColor>(getValues<int>("theme.color"));
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
		path = bf::current_path().parent_path().append("Resources");
		path /= "settings.iridium";
	} else
	{
		path = bf::current_path();
		path /= "settings.iridium";
	}
	return path;
}


void Settings::initSettings()
{
	m_settings.put("theme.color", 0);
	m_settings.put("general.loadType", 0);
	m_settings.put("general.maxDepth", 2);
	m_settings.put("general.maxProcess", std::thread::hardware_concurrency());
	m_settings.put("remotes", "");

	pt::ptree arrayFlags, flag;
	flag.put("--transfers", 4);
	arrayFlags.push_back(std::make_pair("", flag));

	m_settings.add_child("general.flags", arrayFlags);

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
			static_cast<RcloneFileModelDistant::Load>(m_settings.get<uint>("general.loadType")));
		RcloneFileModelDistant::setMaxDepth(m_settings.get<uint16_t>("general.maxDepth"));
		RcloneManager::setMaxProcess(m_settings.get<uint16_t>("general.maxProcess"));
		Settings::changeDirIcon(static_cast<Settings::ThemeColor>(m_settings.get<int>("theme.color")));

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
void Settings::setValues(const std::string &node, const auto &val)
{
	try
	{
		m_settings.put(node, val);
		saveSettings();
	} catch (boost::exception &e)
	{
		cout << "eror set Value" << diagnostic_information_what(e, true) << endl;
	}
}

/**
 * @brief get value from node path in m_settings ptree
 * @tparam Type
 * @param node
 * @return Type
 */
template<class Type>
Type Settings::getValues(const string &node)
{
	try
	{
		return m_settings.get<Type>(node);
	} catch (boost::exception &e)
	{
		cout << "eror get Value" << diagnostic_information_what(e, true) << endl;
	}
	return Type();
}
