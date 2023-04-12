cmake_minimum_required(VERSION 3.24)
project(Iridium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b -D NDEBUG -Wno-deprecated-declarations -O3 ")


# set qt path
set(CMAKE_PREFIX_PATH "/Users/sr-71/Qt/6.6.0/macos/lib/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem json thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets)


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
        src/FileView/TreeWidgets/TreeFileItemDistant.cpp src/FileView/TreeWidgets/TreeFileItemDistant.hpp src/FileView/TreeWidgets/TreeFileViewContainer.cpp src/FileView/TreeWidgets/TreeFileViewContainer.hpp src/Remote/AddNewRemote/Local/LocalRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Local/LocalRemoteConfigParamsFrame.hpp src/MainWindow/ToolBar.cpp
        src/MainWindow/ToolBar.hpp src/Config/Settings.cpp src/Config/Settings.hpp src/Other/RoundedButton.cpp src/Other/RoundedButton.hpp src/Remote/AddNewRemote/Sftp/SftpRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/Sftp/SftpRemoteConfigParamsFrame.hpp src/Other/RoundedLineEdit.cpp src/Other/RoundedLineEdit.hpp src/Task/TaskTreeView.cpp src/Task/TaskTreeView.hpp src/Task/TaskRow.cpp src/Task/TaskRow.hpp)

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
        src/Remote/ListRemote)

SET(TRANSLATION
        languages/iridium_en.ts
        )


set(RCLONE "${CMAKE_CURRENT_SOURCE_DIR}/rclone")


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(COPY ${RCLONE} DESTINATION Iridium.app/Contents/MacOS)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set(myApp_ICON ressources/app.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(Iridium MACOSX_BUNDLE application.qrc ${TRANSLATION} ${myApp_ICON} ${SRC})
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
        )
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(Iridium PRIVATE ws2_32)
endif ()
