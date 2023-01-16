#include <QApplication>
#include "MainWindow/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow app;
    app.show();
    return QApplication::exec();
}
