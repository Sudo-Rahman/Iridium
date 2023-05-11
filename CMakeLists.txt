cmake_minimum_required(VERSION 3.24)
project(Iridium)

set(Iridium_VERSION_MAJOR 0)
set(Iridium_VERSION_MINOR 8)
set(Iridium_VERSION_PATCH 3)
set(Iridium_VERSION "${Iridium_VERSION_MAJOR}.${Iridium_VERSION_MINOR}.${Iridium_VERSION_PATCH}")

configure_file (src/Config/Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Config/Version.h @ONLY)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b -D NDEBUG -Wno-deprecated-declarations -O3")

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

file(GLOB_RECURSE SRC
        src/*.cpp
        src/*.h
        src/*.hpp
        )

include_directories(src
        src/Task
        src/Config
        src/FileView
        src/FileView/TreeWidgets
        src/LeftSideBar
        src/MainWindow
        src/Other
        src/Rclone
        src/Search
        src/Remote
        src/Remote/AddNewRemote
        src/Remote/ListRemote
        src/MainWindow/Menu
        src/MainWindow/Menu/Settings)

file(GLOB_RECURSE TRANSLATION
        languages/iridium_*.ts
        )


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Iridium.app/Contents/Translations)
    set_source_files_properties(${TRANSLATION}
            PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/Iridium.app/Contents/Translations)

else ()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Translations)
    set_source_files_properties(${TRANSLATION} PROPERTIES
            OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/Translations)
endif ()

qt_add_translation(QM_FILES ${TRANSLATION})


set(RCLONE "${CMAKE_CURRENT_SOURCE_DIR}/rclone")


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    file(COPY ${RCLONE} DESTINATION Iridium.app/Contents/MacOS)
    set(MACOSX_BUNDLE_ICON_FILE Iridium.icns)
    set(myApp_ICON ressources/Iridium.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    qt6_add_executable(${PROJECT_NAME} MACOSX_BUNDLE application.qrc ${myApp_ICON} ${SRC} ${QM_FILES})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(COPY ${RCLONE}.exe DESTINATION Iridium/../)
    enable_language("RC")
    set(WIN32_ICO ${CMAKE_CURRENT_SOURCE_DIR}/MyApp.rc)
    qt6_add_executable(${PROJECT_NAME} WIN32 application.qrc ${WIN32_ICO} ${SRC} ${QM_FILES})
else ()
    qt6_add_executable(${PROJECT_NAME} application.qrc ${SRC} ${QM_FILES})
endif ()


target_link_libraries(${PROJECT_NAME} PRIVATE
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
    target_link_libraries(Iridium PRIVATE ws2_32
            -static gcc stdc++ winpthread -dynamic)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_NAME} ${Iridium_VERSION}"
            MACOSX_BUNDLE_COPYRIGHT "© 2023 ${PROJECT_NAME}"
            MACOSX_BUNDLE_ICON_FILE Iridium.icns
            )
endif ()