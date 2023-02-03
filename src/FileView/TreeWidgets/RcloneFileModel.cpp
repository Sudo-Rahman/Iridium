//
// Created by sr-71 on 02/02/2023.
//

#include "RcloneFileModel.hpp"

RcloneFileModel::RcloneFileModel(RcloneFileModel::Type type, const QString &path, QObject *parent) : QStandardItemModel(
	parent), path(path)
{
	setColumnCount(4);
	setRowCount(0);

	setHorizontalHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});

	switch (type)
	{
		case Local:
			initLocal();
			break;
		case Distant:
			initDistant();

	}

}

void RcloneFileModel::initDistant()
{
	auto *drive = new TreeFileItem(path);
	appendRow({
				  drive,
				  new QStandardItem("--"),
				  new QStandardItem("--"),
				  new QStandardItem(tr("Disque"))
			  });
	addItemDistant(path, drive);


}

QList<QStandardItem *> getItemList(TreeFileItem *item)
{
	return {
		item,
		new QStandardItem(item->getFile()->getSizeString()),
		new QStandardItem(item->getFile()->getModTimeString()),
		new QStandardItem(item->getFile()->getIsDirString())
	};
}

void RcloneFileModel::addItemDistant(const QString &path, TreeFileItem *parent)
{
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItem(path, file.toObject());
			parent->appendRow(getItemList(item));
			if (item->getFile()->isDir())
				addItemDistant(item->getFile()->getPath(), item);
		}
	});
	rclone->lsJson(path.toStdString());
}

void RcloneFileModel::initLocal()
{
	m_thread = std::make_shared<std::thread>(
		[=]()
		{
			auto *drive = new TreeFileItem(path);
			appendRow({
						  drive,
						  new QStandardItem("--"),
						  new QStandardItem("--"),
						  new QStandardItem(tr("Disque"))
					  });
		}
	);

}

void RcloneFileModel::addItemLocal(const QString &path, TreeFileItem *parent)
{
	qDebug() << path;
	for(const QFileInfo &info : QDir(path).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries)){
		auto *item = new TreeFileItem(RcloneFile(info.filePath()));
		parent->appendRow(getItemList(item));
	}
}


