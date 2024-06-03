#include "IridiumApp.hpp"
#include <memory>
#include <QPalette>
#include <QThread>

boost::signals2::signal<void()> IridiumApp::onThemeChange;

IridiumApp::IridiumApp(int &argc, char **argv) : QApplication(argc, argv)
{
	if (palette().color(QPalette::Window).lightness() < 128)
		setProperty("dark", true);
}

// void IridiumApp::runOnMainThread(std::function<void()> f)
// {
// 	if (QThread::currentThread() != instance()->thread())
// 	{
// 		QMetaObject::invokeMethod(instance(), f, Qt::QueuedConnection);
// 	}
// 	else { f(); }
// }

void IridiumApp::runOnMainThread(std::function<void()> &&f)
{
	if (QThread::currentThread() != instance()->thread())
	{
		QMetaObject::invokeMethod(instance(), std::move(f), Qt::QueuedConnection);
	}
	else { f(); }
}

bool IridiumApp::event(QEvent *event)
{
	if (event->type() == QEvent::ApplicationPaletteChange)
	{
		if (palette().color(QPalette::Window).lightness() < 128)
			setProperty("dark", true);
		else setProperty("dark", false);
		onThemeChange();
	}
	return QApplication::event(event);
}
