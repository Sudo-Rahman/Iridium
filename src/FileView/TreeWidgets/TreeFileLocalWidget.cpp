//
// Created by sr-71 on 29/01/2023.
//

#include "TreeFileLocalWidget.hpp"
#include <QModelIndex>
#include <QHeaderView>
#include <boost/process.hpp>

TreeFileLocalWidget::TreeFileLocalWidget(QWidget *parent) : QTreeView(parent)
{
	setEditTriggers(QAbstractItemView::AllEditTriggers);
	model = new FileSystemModel;
	QTreeView::setModel(model);

	setAlternatingRowColors(true);

	connect(this, &QTreeView::doubleClicked, this, [=](const QModelIndex &index)
	{
		if (model->fileInfo(index).isDir())
			QTreeView::setRootIndex(index);
		else
		{
			mthread = std::make_shared<std::thread>([=](){
			#if defined(__linux__)
				boost::process::child c("vlc", boost::process::args({model->filePath(index).toStdString()}));
			#elif _WIN32
			#elif __APPLE__
				boost::process::child c("/usr/bin/open", boost::process::args({"-a","VLC.app", model->filePath(index).toStdString()}));
			#endif
			c.wait();
			});
		}
	});


}

FileSystemModel::FileSystemModel(QWidget *parent) : QFileSystemModel(parent)
{
	setRootPath(QDir::currentPath());
	setFilter(QDir::NoDotDot | QDir::AllEntries | QDir::NoDot | QDir::Hidden);
	FileSystemModel::setHeaderData(0,Qt::Horizontal,tr("Noreerggre"),Qt::EditRole);
}

bool FileSystemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	return QAbstractItemModel::setHeaderData(section, orientation, value, role);
}
