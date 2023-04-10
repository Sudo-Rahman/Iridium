#include <QApplication>
#include "MainWindow/MainWindow.hpp"
#include "Config/Settings.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	if(QSysInfo::productType() == "windows")
		QApplication::setStyle("fusion");
	Settings::init();
	MainWindow app;
	app.show();
	return QApplication::exec();
}
