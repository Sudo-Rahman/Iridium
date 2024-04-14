#pragma once

#include <any>
#include <QApplication>

class IridiumApp : public QApplication
{
    Q_OBJECT
public:
    IridiumApp(int &argc, char **argv);

    static void runOnMainThread(std::function<void()> f);
};
