//
// Created by sr-71 on 05/04/2023.
//

#ifndef IRIDIUM_SETTINGS_HPP
#define IRIDIUM_SETTINGS_HPP

#include <QSettings>
#include <QIcon>
#include <QApplication>
#include <Remote.h>


class Settings : public QSettings
{
Q_OBJECT

public:

	enum class ThemeColor
	{
		Default,
		Yellow,
		Green,
		Red,
		Purple,
		Orange,
		Gray,
		Pink,
	};


	static QIcon DIR_ICON;
	static QIcon HARDDRIVE_ICON;

	static void init();

	static void changeDirIcon(const ThemeColor &color);

	static QList<RemoteInfoPtr> getLocalRemotes();

	static void addLocalRemote(const RemoteInfoPtr &remoteInfo);

	static void addLocalRemote(const RemoteInfo &remoteInfo);

	static void deleteRemote(const RemoteInfoPtr &remoteInfo);
};


#endif //IRIDIUM_SETTINGS_HPP
