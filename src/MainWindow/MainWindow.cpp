//
// Created by sr-71 on 09/01/2023.
//

#include "MainWindow.hpp"
#include "../Rclone/Rclone.hpp"

#include <QPushButton>
#include <QLayout>
#include "../Remote/AddNewRemote/ChoseRemoteDialog.hpp"


MainWindow::MainWindow( QWidget * parent ) : QMainWindow( parent )
{
    setWindowTitle( "Iridium" );
    setContentsMargins(0,0,0,0);

    auto *wid = new QWidget(this);
    wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wid->setBackgroundRole(QPalette::Highlight);
     auto  * lay = new QHBoxLayout(wid);
    setCentralWidget(wid);

    auto *t = new ChoseRemoteDialog();
    t->exec();


}
