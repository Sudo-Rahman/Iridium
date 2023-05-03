#include <QApplication>
#include <MainWindow.hpp>
#include <Settings.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Iridium");
    QApplication::QGuiApplication::setWindowIcon(QIcon(":/ressources/Iridium.svg"));
    if (QSysInfo::productType() == "windows")
        QApplication::setStyle("fusion");
    Settings::init();
    MainWindow app;
    app.show();
    return QApplication::exec();
}
