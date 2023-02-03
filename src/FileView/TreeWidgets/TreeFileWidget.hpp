//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEWIDGET_HPP
#define IRIDIUM_TREEFILEWIDGET_HPP

#include <QTreeView>
#include <QStandardItemModel>
#include "../../Rclone/Rclone.hpp"
#include "TreeFileItem.hpp"

class TreeFileWidget : public QTreeView
{
Q_OBJECT
	QString remoteName{};
	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};

public:
	explicit TreeFileWidget(QString remoteName, QWidget *parent = nullptr);

	const QString &getRemoteName() const;

	void back();


protected:
	void resizeEvent(QResizeEvent *event) override;


};

#endif //IRIDIUM_TREEFILEWIDGET_HPP
