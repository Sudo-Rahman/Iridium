//
// Created by Rahman on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEVIEW_HPP
#define IRIDIUM_TREEFILEVIEW_HPP

#include "TreeFileItem.hpp"
#include "RcloneFileModel.hpp"
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <Rclone.hpp>
#include <RoundedLineEdit.hpp>

#include "TaskRowParent.hpp"

class TreeFileView : public QTreeView
{
Q_OBJECT

    RcloneFileModel *_model{};
    std::vector<std::pair<QModelIndex, uint32_t>> _index_back{}, _index_front{};
    RemoteInfoPtr _remote_info{};

    uint64_t _clickTime{};
    QModelIndex _clickIndex{};

    bool _dragable{};

    std::map<std::string, bool> _operations{};

    RoundedLineEdit *_search_line_edit{};

    boost::thread _reload_thread{};
    std::atomic_bool _reloadable{true};

public:
    explicit TreeFileView(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

    explicit TreeFileView(QWidget *parent = nullptr);

    ~TreeFileView() override
    {
        _reload_thread.interrupt();
        _reload_thread.join();
    }

    void back();

    void front();

    void changeRemote(const RemoteInfoPtr &info);

    void copyto(const std::vector<RcloneFilePtr> &, TreeFileItem *item = nullptr);

    void reload(TreeFileItem *item = nullptr);

    [[nodiscard]] RemoteInfoPtr remoteInfo() const { return _remote_info; };

    void search(const QString &text);

    void preview(const TreeFileItem *);

    void set_reloadable(bool r) {_reloadable = r; }

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

    void mkdir();

    QDialog *mkdirDialog();

    void editItem(const QModelIndex &index);

    void rename(const TreeFileItem *item, const QString &newName);

    void showContextMenu();

    void showSearchLine();

    void autoReload();

    void connectProcessreloadable(ir::process *process);

signals:

    void pathChanged(const QString &);

    void pasted(const RcloneFilePtr &);

    void taskAdded(const RcloneFile& src, const RcloneFile& dst, const ir::process_ptr& rclone,
                   TaskRowParent::taskType type = TaskRowParent::Unknown);

    void resized();

    void previewed(const QByteArray &data);

};

#endif //IRIDIUM_TREEFILEVIEW_HPP
