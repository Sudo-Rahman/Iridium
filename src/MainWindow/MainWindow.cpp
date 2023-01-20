//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"

#include <QPushButton>
#include <QtGlobal>
#include <QLayout>
#include "../FileView/TreeWidgets/TreeFileWidget.hpp"
#include <boost/process.hpp>


#include <iostream>
namespace bp = boost::process;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Iridium");
    setContentsMargins(0, 0, 0, 0);
    setMinimumSize(1000, 500);

    auto *wid = new QWidget(this);
    wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wid->setBackgroundRole(QPalette::Highlight);
    auto *lay = new QHBoxLayout(wid);
    setCentralWidget(wid);

    lay->addWidget(new TreeFileWidget("nass:"));

}
