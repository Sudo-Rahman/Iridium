cmake_minimum_required(VERSION 3.24)
project(Iridium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Sql)


set(SRC
        src/main.cpp
        src/Rclone/Rclone.cpp src/Rclone/Rclone.hpp
        src/MainWindow/MainWindow.cpp src/MainWindow/MainWindow.hpp
        src/Rclone/RcloneFile.cpp src/Rclone/RcloneFile.hpp
        src/Remote/AddNewRemote/AddNewRemoteDialog.cpp src/Remote/AddNewRemote/AddNewRemoteDialog.hpp
        src/Remote/AddNewRemote/RemoteFrame/RemoteFrame.cpp src/Remote/AddNewRemote/RemoteFrame/RemoteFrame.hpp
        src/Remote/AddNewRemote/RemoteFrame/RemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/RemoteFrame/RemoteConfigParamsFrame.hpp src/Remote/Remote.h src/Remote/AddNewRemote/RemoteFrame/GoogleDrive/GoogleDriveRemoteConfigParamsFrame.cpp src/Remote/AddNewRemote/RemoteFrame/GoogleDrive/GoogleDriveRemoteConfigParamsFrame.hpp src/FileView/TreeWidgets/TreeFileWidget.cpp src/FileView/TreeWidgets/TreeFileWidget.hpp src/FileView/TreeWidgets/TreeFileItem.cpp src/FileView/TreeWidgets/TreeFileItem.hpp)

SET(TRANSLATION
        languages/iridium_en.ts
        )


set(RCLONE "${CMAKE_CURRENT_SOURCE_DIR}/rclone")


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(COPY ${RCLONE} DESTINATION Iridium.app/Contents/MacOS)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set(myApp_ICON images/app.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(Iridium MACOSX_BUNDLE application.qrc ${TRANSLATION} ${myApp_ICON} ${SRC})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(COPY ${RCLONE} DESTINATION Iridium/../)
    enable_language("RC")
    set(WIN32_ICO ${CMAKE_CURRENT_SOURCE_DIR}/MyApp.rc)
    add_executable(Iridium WIN32 application.qrc ${WIN32_ICO} ${IMAGES} ${SRC})
else ()
    add_executable(Iridium application.qrc ${IMAGES} ${SRC})
endif ()


target_link_libraries(Iridium PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
        )

