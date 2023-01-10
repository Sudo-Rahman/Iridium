cmake_minimum_required(VERSION 3.24)
project(Iridium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Users/sr-71/Qt/6.4.1/macos/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Iridium src/main.cpp src/Rclone/Rclone.cpp src/Rclone/Rclone.hpp src/MainWindow/MainWindow.cpp src/MainWindow/MainWindow.hpp src/Rclone/RcloneFile.cpp src/Rclone/RcloneFile.hpp)
target_link_libraries(Iridium
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

