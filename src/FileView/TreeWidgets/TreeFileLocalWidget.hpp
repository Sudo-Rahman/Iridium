//
// Created by sr-71 on 29/01/2023.
//

#ifndef IRIDIUM_TREEFILELOCALWIDGET_HPP
#define IRIDIUM_TREEFILELOCALWIDGET_HPP

#include <QTreeWidget>
#include <QFileSystemModel>
#include <thread>
#include <memory>


class TreeFileLocalWidget : public QTreeWidget
{
Q_OBJECT
	std::shared_ptr<std::thread> mthread{};

public:
	explicit TreeFileLocalWidget(QWidget *parent = nullptr);


};


#endif //IRIDIUM_TREEFILELOCALWIDGET_HPP
