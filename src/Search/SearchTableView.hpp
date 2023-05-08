//
// Created by sr-71 on 07/05/2023.
//

#ifndef IRIDIUM_SEARCHTABLEVIEW_HPP
#define IRIDIUM_SEARCHTABLEVIEW_HPP


#include <QTableView>
#include <QStandardItemModel>
#include <TreeFileItem.hpp>
#include <boost/json.hpp>
#include <boost/thread.hpp>
#include <Rclone.hpp>


class SearchTableView : public QTableView
{
Q_OBJECT

    QStandardItemModel *m_model{};
    RclonePtr m_rclone{};
    boost::thread m_thread{};

public :
    explicit SearchTableView(QWidget *parent = nullptr);

    void search(const QString &text, const RemoteInfoPtr &remoteInfo);

private:
    void addFile(const boost::json::object &file, const RemoteInfoPtr &remoteInfo);

    void showCustomContextMenu();

    signals:
    void searchFinished();

};


#endif //IRIDIUM_SEARCHTABLEVIEW_HPP
