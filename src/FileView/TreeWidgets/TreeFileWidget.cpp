//
// Created by sr-71 on 18/01/2023.
//
#include <QHeaderView>

#include "TreeFileWidget.hpp"
#include "TreeFileItem.hpp"

TreeFileWidget::TreeFileWidget(QString remoteName, QWidget *parent) : remoteName(std::move(remoteName)),
                                                                      QTreeWidget(parent)
{
    setAlternatingRowColors(true);
    setHeaderLabels({tr("Nom"), tr("Taille"), tr("Date de modification"), tr("Type")});
    header()->setSectionsMovable(true);
    header()->resizeSection(0, 300);
    header()->setStretchLastSection(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    addItem(TreeFileWidget::remoteName, nullptr);

}

const QString &TreeFileWidget::getRemoteName() const
{
    return remoteName;
}

void TreeFileWidget::addItem(const QString &path, QTreeWidgetItem *parent)
{
    auto *rclone = new Rclone;
    list << rclone;
    rclone->lsJson(path);
    connect(rclone, &Rclone::lsJsonFinished, this, [=](const QJsonDocument &doc)
    {
        for (const auto &file: doc.array())
        {
            auto *item = new TreeFileItem(path, file.toObject());
            parent == nullptr ? addTopLevelItem(item) : parent->addChild(item);
            if (item->getFile()->isDir())
            {
                addItem(item->getFile()->getPath(), item);
            }
            c++;
        }
        qDebug() << c;
        rclone->deleteLater();
    });
}

void TreeFileWidget::resizeEvent(QResizeEvent *event)
{
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    QAbstractItemView::resizeEvent(event);
    header()->setSectionResizeMode(0, QHeaderView::Interactive);

}

