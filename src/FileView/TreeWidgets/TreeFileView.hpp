//
// Created by Rahman on 18/01/2023.
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
#include <Global.hpp>
#include <RoundedLineEdit.hpp>

class TreeFileView : public QTreeView
{
Q_OBJECT

    RcloneFileModel *_model{};
    QList<QModelIndex> _index_back{}, _index_front{};
    RemoteInfoPtr _remote_info{};

    uint64_t _clickTime{};
    QModelIndex _clickIndex{};

    QList<TreeFileItem *> _dragItems{};

    bool _dragable{}, _ctrl_presed{};

    RoundedLineEdit *_search_line_edit{};

public:
    explicit TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

    explicit TreeFileView(QWidget *parent = nullptr);

    void back();

    void front();

    void changeRemote(const RemoteInfoPtr &info);

    void copyto(const std::vector<RcloneFilePtr> &, TreeFileItem *item = nullptr);

    void reload(TreeFileItem *item = nullptr);

    [[nodiscard]] RemoteInfoPtr remoteInfo() const { return _remote_info; };

    void search(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;

    bool event(QEvent *event) override;

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

    void showContextMenu();

    void showSearchLine();

signals:

    void pathChanged(const QString &);

    void pasted(const RcloneFilePtr &);

    void
    taskAdded(const QString &src, const QString &dst, const RclonePtr &rclone, const std::function<void()> &callable,
              const Rclone::TaskType &type = Rclone::Unknown);

    void resized();

};

#endif //IRIDIUM_TREEFILEVIEW_HPP
