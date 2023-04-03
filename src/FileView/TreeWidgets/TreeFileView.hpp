//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEVIEW_HPP
#define IRIDIUM_TREEFILEVIEW_HPP

#include <QTreeView>
#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <thread>
#include <QMessageBox>
#include "../../Remote/Remote.h"


class TreeFileView : public QTreeView
{
Q_OBJECT

	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};
	RemoteInfoPtr m_remoteInfo{};

public:
	explicit TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

	void back();

	void front();

	void changeRemote(const RemoteInfoPtr &info);

protected:
	void resizeEvent(QResizeEvent *event) override;

protected slots:

	void doubleClick(const QModelIndex &index);

	virtual void expand(const QModelIndex &index);

	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	void setModel(RcloneFileModel *model);
	QString getPath();

signals:

	void pathChanged(const QString &path);


};

#endif //IRIDIUM_TREEFILEVIEW_HPP
