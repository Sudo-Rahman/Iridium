//
// Created by sr-71 on 14/06/2023.
//

#pragma once

#include <QTableView>
#include <QStandardItemModel>

#include "RcloneFile.hpp"

class SyncTableItem : public QStandardItem
{

};

class SyncTableView : public QTableView
{
Q_OBJECT

    QStandardItemModel *_model{};
    RcloneFilePtr _src{}, _dst{};

public:
    explicit SyncTableView(QWidget *parent = nullptr);

    void setFiles(const RcloneFilePtr &src, const RcloneFilePtr &dst)
    {
        _src = src;
        _dst = dst;
    }

    void analyse();

    void sync();

};