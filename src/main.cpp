#include <IridiumApp.hpp>
#include <MainWindow.hpp>
#include <Settings.hpp>
#include <Config.h>

int main(int argc, char *argv[])
{
    IridiumApp a(argc, argv);
    IridiumApp::setApplicationName("Iridium");
    IridiumApp::setApplicationVersion(IRIDIUM_VERSION.c_str());
    QGuiApplication::setWindowIcon(QIcon(":/ressources/Iridium.svg"));
    // Looks not very pretty on Windows
//    if (QSysInfo::productType() == "windows")
//        QApplication::setStyle("fusion");
    Settings::init();
    MainWindow app;
    app.show();
    return IridiumApp::exec();
}
