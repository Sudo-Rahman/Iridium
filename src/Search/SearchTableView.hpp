//
// Created by sr-71 on 07/05/2023.
//

#pragma once


#include <QTableView>
#include <TreeFileItem.hpp>
#include <boost/json.hpp>
#include <boost/thread.hpp>
#include <QThreadPool>
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
    iridium::rclone::process_pool _pool{10};
    std::vector<boost::thread> _threads{};
    std::map<RemoteInfo *, RcloneFile> _remote_to_root_file{};
    std::atomic_uint8_t _searching = 0;

public :
    explicit SearchTableView(QWidget *parent = nullptr);

    void searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo);

    void searchDistant(const ir::option::vector &filters, const RemoteInfoPtr &remoteInfo);

    void stopAllSearch();

    ~SearchTableView() override
    {
        _pool.stop();
    }

private:
    void addFile(const RcloneFilePtr &file) const;

    void showCustomContextMenu();

    void terminateSearch();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

    void searchFinished();

    void searchStarted();

};