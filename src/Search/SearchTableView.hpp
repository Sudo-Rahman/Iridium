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
#include <QThreadPool>
#include <Rclone.hpp>
#include <iridium/process.hpp>


class SearchTableView : public QTableView
{
Q_OBJECT

    struct Row
    {
        boost::json::object file;
        RemoteInfoPtr remoteInfo;
    };

    QStandardItemModel *_model{};
    std::vector<RclonePtr> _rclones{};
    iridium::rclone::process_pool _pool{10};
    std::vector<boost::thread> _threads{};
    std::unique_ptr<boost::thread> _adder{};
    std::vector<Row> _rows{};
    std::map<RemoteInfo *, RcloneFile> _remote_to_root_file{};
    std::mutex _mutex{};
    std::condition_variable _cv{};
    std::atomic_uint8_t _searching = 0;

public :
    explicit SearchTableView(QWidget *parent = nullptr);

    void searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo);

    void searchDistant(const std::vector<Rclone::Filter> &filters, const RemoteInfoPtr &remoteInfo);

    void stopAllSearch();

    ~SearchTableView() override
    {
        _adder->interrupt();
        _cv.notify_one();
    }

private:
    void addFile(const RcloneFilePtr &file);

    void showCustomContextMenu();

    void terminateSearch();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

    void searchFinished();

    void searchStarted();

};


#endif //IRIDIUM_SEARCHTABLEVIEW_HPP
