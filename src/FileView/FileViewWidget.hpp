//
// Created by Rahman on 30/03/2023.
//

#ifndef IRIDIUM_FILEVIEWWIDGET_HPP
#define IRIDIUM_FILEVIEWWIDGET_HPP

#include <TreeFileViewContainer.hpp>
#include <ListRemoteWidget.hpp>
#include <QSplitter>

class FileViewWidget final : public QWidget
{
Q_OBJECT

private:
    TreeFileViewContainer *_treeFileView1{}, *_treeFileView2{};
    QHBoxLayout *_layout{};

    QSplitter *_splitter{};

    QList<TreeFileItem *> _current_file_list{};

public:
    explicit FileViewWidget(QWidget *parent = nullptr);

    void changeRemote(const std::shared_ptr<remotes_selected> &);


signals:

    void
    taskAdded(const QString &src, const QString &dst, const RclonePtr &rclone, const std::function<void()> &callable,
              const Rclone::TaskType &type = Rclone::Unknown);
};


#endif //IRIDIUM_FILEVIEWWIDGET_HPP
