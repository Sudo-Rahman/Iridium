//
// Created by sr-71 on 29/01/2023.
//

#ifndef IRIDIUM_TREEFILELOCALWIDGET_HPP
#define IRIDIUM_TREEFILELOCALWIDGET_HPP

#include <QTreeView>
#include <QFileSystemModel>
#include <thread>
#include <memory>


class FileSystemModel : public QFileSystemModel
{
public:
	explicit FileSystemModel(QWidget *parent = nullptr);

	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
};

class TreeFileLocalWidget : public QTreeView
{
Q_OBJECT
	FileSystemModel *model{};
	std::shared_ptr<std::thread> mthread{};

public:
	explicit TreeFileLocalWidget(QWidget *parent = nullptr);


};


#endif //IRIDIUM_TREEFILELOCALWIDGET_HPP
