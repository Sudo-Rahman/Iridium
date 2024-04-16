#include <IridiumApp.hpp>
#include <MainWindow.hpp>
#include <Settings.hpp>
#include <Config.h>
#include <QMessageBox>

#include "RcloneNotFoundWidget.hpp"

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
	Settings::initRlclone([](bool ok)
	{
		if (not ok)
		{
			auto not_found = RcloneNotFoundWidget();
			not_found.exec();
			if(not not_found.rcloneInstalled())
				exit(1);
		}
	});
	MainWindow app;
	app.show();
	return IridiumApp::exec();
}
