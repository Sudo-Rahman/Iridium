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

	QString remoteName{};
	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};
	Remote type{};
	RemoteInfo m_remoteInfo{};

public:
	explicit TreeFileView(Remote type, QString remoteName, QWidget *parent = nullptr);

	void back();

	void front();

	void changeRemote(const RemoteInfo &info);

protected:
	void resizeEvent(QResizeEvent *event) override;

protected slots:

	void doubleClick(const QModelIndex &index);

	virtual void expand(const QModelIndex &index);

	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	void setModel(RcloneFileModel *model);


};

#endif //IRIDIUM_TREEFILEVIEW_HPP
