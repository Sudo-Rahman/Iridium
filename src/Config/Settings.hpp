//
// Created by Rahman on 05/04/2023.
//

#ifndef IRIDIUM_SETTINGS_HPP
#define IRIDIUM_SETTINGS_HPP

#include <QSettings>
#include <QIcon>
#include <QTranslator>
#include <QApplication>
#include <RcloneFileModelDistant.hpp>
#include <Remote.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>


class Settings
{

public:

    enum ThemeColor
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

    enum Node
    {
        All,
        General,
        Rclone,
        Theme,
        MaxProcess,
        LoadType,
        DirIconColor,
        Remotes,
        MaxDepth,
        Flags,
        Language
    };

private:
    static boost::property_tree::ptree m_settings;
    static const std::map<Node, std::string> m_nodes;

    static boost::property_tree::ptree m_default;

    static void loadSettings();

    static boost::filesystem::path getPathSettings();

    static void initSettings();

    static QIcon DIR_ICON;

    static QIcon HARDDRIVE_ICON;
public:


    static void init();

    static void changeDirIcon(const ThemeColor &color);

    static void saveSettings();

    static QIcon dirIcon() { return DIR_ICON; }

    static QIcon hardDriveIcon() { return HARDDRIVE_ICON; }

    static std::vector<RemoteInfoPtr> getLocalRemotes();

    static std::vector<RemoteInfoPtr> getRemotes();

    static void addLocalRemote(const RemoteInfo &remoteInfo);

    static void deleteRemote(const RemoteInfoPtr &remoteInfo);

    static void resetSettings(const Node &);

    static void initValues();

    static void setValue(const Node &node, const auto &value);

    /**
     * @brief get the value of a node
     * @tparam Type
     * @param node
     * @return
     */
    template<class Type>
    inline
    static Type getValue(const Node &node)
    {
        return m_settings.get_child(m_nodes.at(node)).BOOST_NESTED_TEMPLATE get_value<Type>();
    }

    static void setRcloneFlag(const Rclone::Flag &, const std::string &);

    static std::string getRcloneFlag(const Rclone::Flag &);

    static void setLanguage(const QLocale::Language &);

};


#endif //IRIDIUM_SETTINGS_HPP
