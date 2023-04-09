//
// Created by sr-71 on 05/04/2023.
//

#include "Settings.hpp"
#include "../Rclone/Rclone.hpp"

using namespace std;


QIcon Settings::DIR_ICON;
QIcon Settings::HARDDRIVE_ICON;
string Rclone::m_pathRclone;


void Settings::changeDirIcon(const Settings::ThemeColor &color)
{
	const QIcon YELLOW_DIR_ICO = QIcon::fromTheme("yellow-folder");
	const QIcon GREEN_DIR_ICO = QIcon::fromTheme("green-folder");
	const QIcon RED_DIR_ICO = QIcon::fromTheme("red-folder");
	const QIcon PURPLE_DIR_ICO = QIcon::fromTheme("purple-folder");
	const QIcon ORANGE_DIR_ICO = QIcon::fromTheme("orange-folder");
	const QIcon GRAY_DIR_ICO = QIcon::fromTheme("gray-folder");
	const QIcon PINK_DIR_ICO = QIcon::fromTheme("pink-folder");
	const QIcon DEFAULT_DIR_ICO = QIcon::fromTheme("default-folder");

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
}

QList<RemoteInfoPtr> Settings::getLocalRemotes()
{
	auto *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
	settings->beginGroup("LocalRemotes");
	auto remotes = settings->childKeys();
	QList<RemoteInfoPtr> remoteInfos;

	for (const auto &remote: remotes)
	{
		auto remoteInfo = std::make_shared<RemoteInfo>(
			settings->value(remote).toString().toStdString(),
			RemoteType::LocalHardDrive,
			remote.toStdString()
		);
		remoteInfos << remoteInfo;
	}
	return remoteInfos;
}

void Settings::addLocalRemote(const RemoteInfoPtr &remoteInfo)
{
	auto *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
	settings->beginGroup("LocalRemotes");
	settings->setValue(QString::fromStdString(remoteInfo->name()), QString::fromStdString(remoteInfo->m_path));
}

void Settings::addLocalRemote(const RemoteInfo &remoteInfo)
{
	std::cout << remoteInfo.name() << remoteInfo.m_path << std::endl;
	auto *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
	settings->beginGroup("LocalRemotes");
	settings->setValue(QString::fromStdString(remoteInfo.name()), QString::fromStdString(remoteInfo.m_path));
}

void Settings::init()
{
	Rclone::setPathRclone(qApp->applicationDirPath().append("/rclone").toStdString());

	Settings::HARDDRIVE_ICON = QIcon::fromTheme("drive-harddisk-solidstate");

	auto *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Iridium", "Iridium");
	settings->beginGroup("Settings");
	auto color = settings->value("Theme", 0).toString();
	Settings::changeDirIcon(static_cast<Settings::ThemeColor>(color.toInt()));
}
