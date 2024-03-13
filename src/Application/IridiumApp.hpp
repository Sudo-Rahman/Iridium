#pragma once

#include <QApplication>

class IridiumApp : public QApplication
{
    Q_OBJECT
public:
    IridiumApp(int &argc, char **argv);

    static void runOnMainThread(std::function<void()> f);
};
