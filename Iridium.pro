QT += core gui widgets

TARGET = Iridium
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

ICON = images/app.icns
RC_ICONS = images/app.ico

SOURCES += \
        src/main.cpp \
        src/Rclone/Rclone.cpp \
        src/MainWindow/MainWindow.cpp \
        src/Rclone/RcloneFile.cpp \
        src/AddNewRemote/ChoseRemoteDialog.cpp \
        src/AddNewRemote/RemoteFrame/RemoteFrame.cpp \
        src/AddNewRemote/RemoteFrame/GoogleDriveRemoteFrame.cpp \



HEADERS += \
        src/Rclone/Rclone.hpp \
        src/MainWindow/MainWindow.hpp \
        src/Rclone/RcloneFile.hpp \
        src/AddNewRemote/ChoseRemoteDialog.hpp \
        src/AddNewRemote/RemoteFrame/RemoteFrame.hpp \
        src/AddNewRemote/RemoteFrame/GoogleDriveRemoteFrame.hpp \



RESOURCES += \
    application.qrc

CONFIG += boost
CONFIG -= console


CONFIG += file_copies
COPIES += rclone
rclone.files = rclone
macx {
    rclone.path = $$OUT_PWD/Iridium.app/Contents/MacOS/
}
win32 {
    rclone.path = $$OUT_PWD/
}




