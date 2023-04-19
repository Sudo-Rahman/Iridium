//
// Created by sr-71 on 05/04/2023.
//

#ifndef IRIDIUM_SETTINGS_HPP
#define IRIDIUM_SETTINGS_HPP

#include <QSettings>
#include <QIcon>
#include <QApplication>
#include <RcloneFileModelDistant.hpp>
#include <Remote.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>


class Settings : public QSettings
{
Q_OBJECT

public:

	enum class ThemeColor
	{
		Default,
		Green,
		Gray,
		Orange,
		Pink,
		Purple,
		Red,
		Yellow,
	};

private:
	static boost::property_tree::ptree m_settings;

	static void loadSettings();

	static boost::filesystem::path getPathSettings();

	static void initSettings();

public:
	static QIcon DIR_ICON;

	static QIcon HARDDRIVE_ICON;

	static void init();

	static void changeDirIcon(const ThemeColor &color);

	static void saveSettings();

	static ThemeColor getDirIconColor();

	static QList<RemoteInfoPtr> getLocalRemotes();

	static void addLocalRemote(const RemoteInfo &remoteInfo);

	static void deleteRemote(const RemoteInfoPtr &remoteInfo);

	static void resetSettings();

	static void initValues();

	static void setValues(const std::string &node, const auto &value);

	template<class Type>
	static Type getValues(const std::string &node);

};


#endif //IRIDIUM_SETTINGS_HPP
