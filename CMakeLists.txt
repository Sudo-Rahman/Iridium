cmake_minimum_required(VERSION 3.24)
project(Iridium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b -D NDEBUG -Wno-deprecated-declarations -O3 ")


# set qt path

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem json thread chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        LinguistTools)

message(STATUS "Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")

set(SRC
        src/main.cpp
        src/Rclone/Rclone.cpp src/Rclone/Rclone.hpp
        src/MainWindow/MainWindow.cpp src/MainWindow/MainWindow.hpp
        src/Rclone/RcloneFile.cpp src/Rclone/RcloneFile.hpp
        src/Remote/AddNewRemote/AddNewRemoteDialog.cpp src/Remote/AddNewRemote/AddNewRemoteDialog.hpp
        src/Remote/AddNewRemote/RemoteWidgetParam.cpp src/Remote/AddNewRemote/RemoteWidgetParam.hpp
        src/Remote/AddNewRemote/RemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/RemoteConfigParamsFrame.hpp
        src/Remote/Remote.h src/Remote/AddNewRemote/GoogleDrive/GoogleDriveRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/GoogleDrive/GoogleDriveRemoteConfigParamsFrame.hpp
        src/FileView/TreeWidgets/TreeFileView.cpp src/FileView/TreeWidgets/TreeFileView.hpp src/FileView/TreeWidgets/TreeFileItem.cpp src/FileView/TreeWidgets/TreeFileItem.hpp
        src/FileView/TreeWidgets/RcloneFileModel.cpp src/FileView/TreeWidgets/RcloneFileModel.hpp src/FileView/TreeWidgets/RcloneFileModelDistant.cpp src/FileView/TreeWidgets/RcloneFileModelDistant.hpp src/FileView/TreeWidgets/ItemMenu.cpp src/FileView/TreeWidgets/ItemMenu.hpp
        src/FileView/TreeWidgets/ItemInfoDialog.cpp src/FileView/TreeWidgets/ItemInfoDialog.hpp src/Utility/Utility.cpp src/Utility/Utility.hpp src/FileView/TreeWidgets/RcloneProxy.cpp src/FileView/TreeWidgets/RcloneProxy.hpp src/Remote/ListRemote/ListRemoteWidget.cpp src/Remote/ListRemote/ListRemoteWidget.hpp
        src/Remote/ListRemote/RemoteWidget.cpp src/Remote/ListRemote/RemoteWidget.hpp src/FileView/TreeWidgets/RcloneFileModelLocal.cpp src/FileView/TreeWidgets/RcloneFileModelLocal.hpp src/FileView/FileViewWidget.cpp src/FileView/FileViewWidget.hpp src/FileView/TreeWidgets/TreeFileItemLocal.cpp src/FileView/TreeWidgets/TreeFileItemLocal.hpp
        src/FileView/TreeWidgets/TreeFileItemDistant.cpp src/FileView/TreeWidgets/TreeFileItemDistant.hpp src/FileView/TreeWidgets/TreeFileViewContainer.cpp src/FileView/TreeWidgets/TreeFileViewContainer.hpp src/Remote/AddNewRemote/Local/LocalRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Local/LocalRemoteConfigParamsFrame.hpp
        src/Config/Settings.cpp src/Config/Settings.hpp src/Other/RoundedButton.cpp src/Other/RoundedButton.hpp src/Remote/AddNewRemote/Sftp/SftpRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Sftp/SftpRemoteConfigParamsFrame.hpp src/Other/RoundedLineEdit.cpp src/Other/RoundedLineEdit.hpp src/Task/TaskTreeView.cpp src/Task/TaskTreeView.hpp src/Task/TaskRow.cpp src/Task/TaskRow.hpp src/MainWindow/Menu/MenuBar.cpp src/MainWindow/Menu/MenuBar.hpp src/Config/Config.h src/Remote/AddNewRemote/OneDrive/OneDriveRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/OneDrive/OneDriveRemoteConfigParamsFrame.hpp src/Remote/AddNewRemote/DropBox/DropboxRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/DropBox/DropboxRemoteConfigParamsFrame.hpp src/Remote/AddNewRemote/Ftp/FtpRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Ftp/FtpRemoteConfigParamsFrame.hpp src/Remote/AddNewRemote/Mega/MegaRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Mega/MegaRemoteConfigParamsFrame.hpp src/MainWindow/Menu/SettingsDialog.cpp src/MainWindow/Menu/SettingsDialog.hpp src/Remote/AddNewRemote/OpenDrive/OpenDriveRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/OpenDrive/OpenDriveRemoteConfigParamsFrame.hpp src/Other/ProgressBar.cpp src/Other/ProgressBar.hpp src/Remote/AddNewRemote/Pcloud/PcloudRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Pcloud/PcloudRemoteConfigParamsFrame.hpp src/MainWindow/Menu/Settings/FolderColorGroupBox.cpp src/MainWindow/Menu/Settings/FolderColorGroupBox.hpp src/MainWindow/Menu/Settings/AppearanceFrame.cpp src/MainWindow/Menu/Settings/AppearanceFrame.hpp src/MainWindow/Menu/Settings/GeneralFrame.cpp src/MainWindow/Menu/Settings/GeneralFrame.hpp src/MainWindow/Menu/Settings/SettingsButton.cpp src/MainWindow/Menu/Settings/SettingsButton.hpp)

include_directories(src
        src/Task
        src/Config
        src/FileView
        src/FileView/TreeWidgets
        src/MainWindow
        src/Other
        src/Rclone
        src/Remote
        src/Remote/AddNewRemote
        src/Remote/ListRemote
        src/MainWindow/Menu
        src/MainWindow/Menu/Settings)

SET(TRANSLATION
        languages/iridium_en.ts
        )

#qt_create_translation(QM_FILES
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
#        ${TRANSLATION})

## update translation
#qt_add_translation(QM_FILES ${TRANSLATION})


set(RCLONE "${CMAKE_CURRENT_SOURCE_DIR}/rclone")


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(COPY ${RCLONE} DESTINATION Iridium.app/Contents/MacOS)
    set(MACOSX_BUNDLE_ICON_FILE Iridium.icns)
    set(myApp_ICON ressources/Iridium.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(Iridium MACOSX_BUNDLE application.qrc ${myApp_ICON} ${SRC} ${QM_FILES})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(COPY ${RCLONE}.exe DESTINATION Iridium/../)
    enable_language("RC")
    set(WIN32_ICO ${CMAKE_CURRENT_SOURCE_DIR}/MyApp.rc)
    add_executable(Iridium WIN32 application.qrc ${WIN32_ICO} ${SRC})
else ()
    add_executable(Iridium application.qrc ${SRC})
endif ()


target_link_libraries(Iridium PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Boost::system
        Boost::filesystem
        Boost::thread
        Boost::json
        Boost::chrono
        )
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(Iridium PRIVATE ws2_32)
endif ()
