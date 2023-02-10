//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEVIEW_HPP
#define IRIDIUM_TREEFILEVIEW_HPP

#include <QTreeView>
#include <QStandardItemModel>
#include "../../Rclone/Rclone.hpp"
#include "TreeFileItem.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <thread>
#include <QMessageBox>

class TreeFileView : public QTreeView
{
Q_OBJECT

	QString remoteName{};
	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};
	Remote type{};

public:
	explicit TreeFileView(Remote type, QString remoteName, QWidget *parent = nullptr);

	[[nodiscard]] const QString &getRemoteName() const;

	void back();

	void front();

protected:
	void resizeEvent(QResizeEvent *event) override;

protected slots:

	void doubleClick(const QModelIndex &index);

	virtual void expand(const QModelIndex &index);

	virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
	void setModel(RcloneFileModel *model);


};

#endif //IRIDIUM_TREEFILEVIEW_HPP
