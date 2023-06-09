#include <QApplication>
#include <MainWindow.hpp>
#include <Settings.hpp>
#include <Config.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Iridium");
    QApplication::setApplicationVersion(IRIDIUM_VERSION.c_str());
    QApplication::QGuiApplication::setWindowIcon(QIcon(":/ressources/Iridium.svg"));
    // Looks not very pretty on Windows
//    if (QSysInfo::productType() == "windows")
//        QApplication::setStyle("fusion");
    Settings::init();
    MainWindow app;
    app.show();
    return QApplication::exec();
}
