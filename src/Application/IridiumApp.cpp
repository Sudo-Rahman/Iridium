#include "IridiumApp.hpp"
#include <memory>

IridiumApp::IridiumApp(int &argc, char **argv) : QApplication(argc, argv)
{}

void IridiumApp::runOnMainThread(std::function<void()> f)
{
    QMetaObject::invokeMethod(qGuiApp, [func = std::move(f)]() { func(); });

}


