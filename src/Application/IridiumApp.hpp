#pragma once

#include <any>
#include <QApplication>
#include <boost/signals2/signal.hpp>

class IridiumApp : public QApplication
{
	Q_OBJECT

public:
	IridiumApp(int &argc, char **argv);

	// static void runOnMainThread(std::function<void()> f);
	static void runOnMainThread(std::function<void()> &&f);

	static boost::signals2::signal<void()> onThemeChange;

	bool event(QEvent *event) override;
};
