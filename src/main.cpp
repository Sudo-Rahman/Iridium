#include <IridiumApp.hpp>
#include <MainWindow.hpp>
#include <Settings.hpp>
#include <Config.h>
#include <RcloneNotFoundWidget.hpp>

int main(int argc, char *argv[])
{
	IridiumApp app(argc, argv);
	IridiumApp::setApplicationName("Iridium");
	IridiumApp::setApplicationVersion(IRIDIUM_VERSION);
	QGuiApplication::setWindowIcon(QIcon(":/resources/Iridium.svg"));

	Settings::init();
	Settings::initRlclone([](bool ok)
	{
		if (not ok)
		{
			auto not_found = RcloneNotFoundWidget();
			not_found.exec();
			if (not not_found.rcloneInstalled())
				exit(0);
		}
	});

	MainWindow main_window;
	main_window.show();

	return IridiumApp::exec();
}
