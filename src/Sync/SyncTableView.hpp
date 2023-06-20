//
// Created by sr-71 on 14/06/2023.
//

#ifndef IRIDIUM_SYNCTABLEVIEW_HPP
#define IRIDIUM_SYNCTABLEVIEW_HPP


#include <QTableView>
#include <QStandardItemModel>
#include <Rclone.hpp>

class SyncTableItem : public QStandardItem
{
    RcloneFilePtr _file;
public:
    explicit SyncTableItem(const RcloneFilePtr &file) { _file = file; }

    [[nodiscard]] const RcloneFilePtr &getFile() const { return _file; }
};

class SyncTableView : public QTableView
{
Q_OBJECT

    QStandardItemModel *_model{};
    RcloneUniquePtr _analyse{}, _sync{};
    RcloneFilePtr _src{}, _dst{};

public:
    explicit SyncTableView(QWidget *parent = nullptr);

    void setFiles(const RcloneFilePtr &src, const RcloneFilePtr &dst){
        _src = src;
        _dst = dst;
    }

    void analyse();

    void sync();

private:


};


#endif //IRIDIUM_SYNCTABLEVIEW_HPP
