//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEVIEW_HPP
#define IRIDIUM_TREEFILEVIEW_HPP

#include <QTreeView>
#include <QTimer>
#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <thread>
#include <QMessageBox>
#include <Rclone.hpp>


class TreeFileView : public QTreeView
{
Q_OBJECT

	QStandardItemModel *model{};
	QList<QModelIndex> indexBack{};
	QList<QModelIndex> indexTop{};
	RemoteInfoPtr m_remoteInfo{};

	uint64_t m_clickTime{};
	QModelIndex m_clickIndex{};

	TreeFileItem *m_editingItem{};

public:
	explicit TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

	explicit TreeFileView(QWidget *parent = nullptr);

	void back();

	void front();

	void changeRemote(const RemoteInfoPtr &info);

	void copyto(const QList<TreeFileItem *> &);

	void reload();

	RemoteInfoPtr remoteInfo() const
	{ return m_remoteInfo; };


protected:
	void resizeEvent(QResizeEvent *event) override;

	void keyPressEvent(QKeyEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;

	void mouseReleaseEvent(QMouseEvent *event) override;

protected slots:

	void doubleClick(const QModelIndex &index);

	void expand(const QModelIndex &index);


private:

	void initUI();

	QString getPath();

	QList<TreeFileItem *> getSelectedItems(bool can_be_empty = false);

	void connectSignals();

	void deleteFile(const QList<TreeFileItem *> &items);

	void removeItem(TreeFileItem *item);

	static bool fileIsInFolder(const QString &name, TreeFileItem *folder);

	void mkdir();

	QDialog *mkdirDialog();

	void editItem(const QModelIndex &index);

	void rename(const TreeFileItem *item, const QString &newName);

signals:

	void pathChanged(const QString &);

	void fileCopied(const QList<TreeFileItem *> &);

	void pasted(const RcloneFilePtr &);

	void
	taskAdded(const QString &src, const QString &dst, const RclonePtr &rclone, const std::function<void()> &callable,
			  const Rclone::TaskType &type = Rclone::Unknown);

};

#endif //IRIDIUM_TREEFILEVIEW_HPP
