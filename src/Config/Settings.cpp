//
// Created by Rahman on 05/04/2023.
//

#include "Settings.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/dll.hpp>
#include <fstream>
#include <QTranslator>
#include <QLocale>
#include <iostream>
#include <Global.hpp>
#include <iridium/process.hpp>
#include <iridium/entities/remote.hpp>

#include "AddNewRemoteDialog.hpp"

using namespace std;
using namespace Iridium;
using namespace boost;
namespace bf = boost::filesystem;
namespace pt = boost::property_tree;
using namespace iridium::rclone;
using namespace iridium::rclone::entities;

QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
property_tree::ptree Settings::_settings, Settings::_default;
signals2::signal<void()> Settings::list_remote_changed;
QTranslator Settings::_translator;

map<Settings::ProcessOptions, iro::basic_opt_uptr> Settings::_options_process = {};

const map<Settings::Node, string> Settings::_nodes = {
				{Settings::Node::All, ""},
				{Settings::Node::General, "general"},
				{Settings::Node::Rclone, "rclone"},
				{Settings::Node::RclonePath, "rclone.path"},
				{Settings::Node::Theme, "theme"},
				{Settings::Node::MaxProcess, "rclone.max_process"},
				{Settings::Node::LoadType, "rclone.load_type"},
				{Settings::Node::DirIconColor, "theme.dir_color"},
				{Settings::Node::Remotes, "remotes"},
				{Settings::Node::MaxDepth, "rclone.max_depth"},
				{Settings::Node::Flags, "rclone.flags"},
				{Settings::Node::Language, "general.language"},
				{Settings::Node::Width, "general.size.width"},
				{Settings::Node::Height, "general.size.height"},
				{Settings::Node::ReloadTime, "general.reload_time"},
		};

const map<Settings::ProcessOptions, string> Settings::_nodes_options = {
				{Transfers, _nodes.at(Flags)+".transfers"},
				{Stats, _nodes.at(Flags)+".stats"},
		};

const map<std::string, QLocale::Language> Settings::languages = {
				{"English", QLocale::English},
				{"Français", QLocale::French}};


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
				remote.first, remote::none, remote.second.get<string>("path")));
		}
		catch (wrapexcept<property_tree::ptree_bad_path> &) {}
	}
	return remotes;
}

void Settings::refreshRemotesList()
{
	std::vector<RemoteInfoPtr> new_remotes;
	iridium::rclone::process().list_remotes([&new_remotes](const std::vector<remote_ptr> &remotes)
	{
		for (const auto &remote: remotes)
			new_remotes.push_back(std::make_shared<RemoteInfo>(remote->name(), remote->type(), remote->path()));
	}).execute().wait_for_finish();
	auto locals = Settings::getLocalRemotes();
	//    insert remotes in refresh
	new_remotes.insert(new_remotes.begin(), locals.begin(), locals.end());
	auto remotes = Global::remotes;

	// remove remotes in remotes but not in new_remotes
	for (const auto &remote: *(&Global::remotes))
	{
		if (std::ranges::none_of(new_remotes.begin(), new_remotes.end(), [&remote](const RemoteInfoPtr &r)
		{
			return *r == *remote;
		}))
			std::erase_if(remotes, [remote](const RemoteInfoPtr &r) { return *r == *remote; });
	}
	// add remotes in new_remotes but not in remotes
	for (const auto &remote: new_remotes)
	{
		if (std::ranges::none_of(remotes.begin(), remotes.end(), [&remote](const RemoteInfoPtr &r)
		{
			return *r == *remote;
		}))
			remotes.push_back(remote);
	}
	Global::remotes = remotes;
	list_remote_changed();
}

/**
 * @brief add a local remote and save it
 * @param remoteInfo
 */
void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
	pt::ptree remote, ptree_path;
	ptree_path.put("", remoteInfo.name());
	remote.add_child("path", ptree_path);

	pt::ptree array = _settings.get_child(_nodes.at(Remotes));
	array.push_back(std::make_pair(remoteInfo.full_path(), remote));
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
	Settings::HARDDRIVE_ICON = QIcon::fromTheme("drive-harddisk-solidstate");
	initSettings();
	loadSettings();
	Settings::changeDirIcon(static_cast<ThemeColor>(getValue<uint8_t>(DirIconColor)));

	boost::filesystem::path translation_dir;
	if (getSystem().os == Os::MacOs)
	{
		translation_dir = dll::program_location().parent_path().parent_path().append("Translations");
	}
	else
		translation_dir = dll::program_location().parent_path().append("Translations");

	auto ok = _translator.load(QLocale(getValue<string>(Language).c_str()), "iridium", "_",
	                           translation_dir.string().c_str());
	QApplication::installTranslator(&_translator);
	QLocale::setDefault(QLocale(getValue<string>(Language).c_str()));
}

void Settings::initRlclone(std::function<void(bool)> &&rclone_init_ok)
{
	// init rclone path

	bf::path rclonePath;
	if(not _settings.get<string>(_nodes.at(RclonePath)).empty())
		rclonePath = _settings.get<string>(_nodes.at(RclonePath));
	else rclonePath = boost::process::search_path(rcloneBaseName().toStdString());

	if (rclonePath.empty())
		rclonePath = dll::program_location().parent_path().append(rcloneBaseName().toStdString());

	Global::path_rclone = rclonePath.string();
	rclone_init_ok(ir::process::initialize(Global::path_rclone));
	_settings.put(_nodes.at(RclonePath), Global::path_rclone);
}

auto Settings::rcloneBaseName() -> QString
{
	return QSysInfo::productType() == "windows" ? "rclone.exe" : "rclone";
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
	}
	else
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
	write_json(getPathSettings().string(), _settings);
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
	}
	else
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
	_settings.put(_nodes.at(RclonePath), "");
	_settings.put(_nodes.at(MaxProcess), std::thread::hardware_concurrency());
	_settings.put(_nodes.at(Remotes), "");
	_settings.put(_nodes.at(Flags), "");
	_settings.put(_nodes.at(Width), 1200);
	_settings.put(_nodes.at(Height), 600);
	_settings.put(_nodes.at(ReloadTime), 10);

	pt::ptree remote, ptree_path;
	RemoteInfo remoteInfo = {"Local", remote::none, "/"};
	ptree_path.put("", remoteInfo.full_path());
	remote.add_child("path", ptree_path);

	pt::ptree array = _settings.get_child(_nodes.at(Remotes));
	array.push_back(std::make_pair(remoteInfo.name(), remote));
	_settings.put_child(_nodes.at(Remotes), array);

	_settings.put_child(_nodes_options.at(Transfers), pt::ptree{"4"});
	_settings.put_child(_nodes_options.at(Stats), pt::ptree{"0.5"});


	_default = _settings;
}

void Settings::resetSettings(const Node &node)
{
	if (node == All)
	{
		_settings.clear();
		_settings = _default;
	}
	else
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
		_options_process[Transfers] = option::performance::transfers(_settings.get<int>(_nodes_options.at(Transfers),4));
		_options_process[Stats] = option::logging::stats(_settings.get<std::string>(_nodes_options.at(Stats),"0.5"));
		for (const auto &option: _options_process)
			ir::process::add_global_option(std::make_unique<iro::basic_option>(*option.second));
		Global::load_type = static_cast<Iridium::Load>(getValue<uint8_t>(LoadType));
		Global::max_depth = getValue<uint8_t>(MaxDepth);
		Global::max_process = getValue<uint8_t>(MaxProcess);
		if (getValue<string>(RclonePath).contains("rclone"))
			Global::path_rclone = getValue<string>(RclonePath);
		Settings::changeDirIcon(static_cast<Settings::ThemeColor>(getValue<uint8_t>(DirIconColor)));
		Global::reload_time = getValue<uint8_t>(ReloadTime);
	}
	catch (boost::exception &e) { cout << diagnostic_information_what(e, true) << endl; }
}

void Settings::setProcessOptions(const ProcessOptions &option, iro::basic_opt_uptr &&value)
{
	_settings.put(_nodes_options.at(option), value->value().value_or(""));
	saveSettings();
	_options_process[option] = std::move(value);
	ir::process::clear_global_options();
	for (const auto &option: _options_process)
		ir::process::add_global_option(std::make_unique<iro::basic_option>(*option.second));
}

void Settings::setProcessOptions(const ProcessOptions &option, const iro::basic_option &value)
{
	setProcessOptions(option,std::make_unique<iro::basic_option>(value));
}

iro::basic_option Settings::getProcessOptions(const ProcessOptions &option)
{
	if (_options_process.contains(option)) return *_options_process[option];
	return {};
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
