//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEVIEW_HPP
#define IRIDIUM_TREEFILEVIEW_HPP

#include <QTreeView>
#include <QStandardItemModel>
#include "../../Rclone/Rclone.hpp"
#include "TreeFileItem.hpp"

class TreeFileView : public QTreeView
{
Q_OBJECT
	QString remoteName{};
	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};

public:
	explicit TreeFileView(QString remoteName, QWidget *parent = nullptr);

	const QString &getRemoteName() const;

	void back();


protected:
	void resizeEvent(QResizeEvent *event) override;


};

#endif //IRIDIUM_TREEFILEVIEW_HPP
