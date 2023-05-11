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
    std::vector<RclonePtr> m_rclones{};
    std::vector<boost::thread> m_threads{};
    std::atomic_uint8_t m_searching = 0;

public :
    explicit SearchTableView(QWidget *parent = nullptr);

    void searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo);

    void searchDistant(const std::vector<Rclone::Filter> &filters, const RemoteInfoPtr &remoteInfo);

    void stopAllSearch();

private:
    void addFile(const boost::json::object &file, const RemoteInfoPtr &remoteInfo);

    void showCustomContextMenu();

    void terminateSearch();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

    void searchFinished();

    void searchStarted();

};


#endif //IRIDIUM_SEARCHTABLEVIEW_HPP
