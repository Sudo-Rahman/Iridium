//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>

#include "TreeFileWidget.hpp"
#include "TreeFileItem.hpp"
#include <thread>
#include <QMessageBox>

TreeFileWidget::TreeFileWidget(QString remoteName, QWidget *parent) : remoteName(std::move(remoteName)),
																	  QTreeWidget(parent)
{
	setAlternatingRowColors(true);
	setHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
	header()->setSectionsMovable(true);
	header()->resizeSection(0, 300);
	header()->setStretchLastSection(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


	manager.allFinished.connect([=](){emit loadDataFinished();});
	addItem(TreeFileWidget::remoteName, nullptr);


}

const QString &TreeFileWidget::getRemoteName() const
{
	return remoteName;
}

void TreeFileWidget::addItem(const QString &path, TreeFileItem *parent)
{
	auto rclone = manager.get();
	connect(rclone.get(), &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
	{
		for (const auto &file: doc.array())
		{
			auto *item = new TreeFileItem(path, file.toObject(), parent == nullptr ? nullptr : parent);
			parent == nullptr ? addTopLevelItem(item) : parent->addChild(item);
			if (item->getFile()->isDir())
				addItem(item->getFile()->getPath(), item);
		}
	});
	rclone->lsJson(path.toStdString());
}

void TreeFileWidget::resizeEvent(QResizeEvent *event)
{
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	header()->setSectionResizeMode(0, QHeaderView::Interactive);

}

