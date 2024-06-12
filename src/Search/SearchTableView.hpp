//
// Created by sr-71 on 07/05/2023.
//

#pragma once


#include <QTableView>
#include <thread>
#include <boost/json.hpp>
#include <boost/thread.hpp>
#include <QThreadPool>
#include <SearchTableModel.hpp>
#include <iridium/process.hpp>


class SearchTableView : public QTableView
{
Q_OBJECT

    SearchTableModel *_model{};
    std::vector<SearchRow *> _data{};
    uint32_t _refresh_model = false;
    iridium::rclone::process_pool _pool{10};
    std::vector<boost::thread> _threads{};
    std::map<RemoteInfo *, RcloneFile> _remote_to_root_file{};
    std::atomic_uint8_t _searching = 0;
    std::mutex _mutex;

    std::vector<QWidget *> _search_info_widgets{};

public :
    explicit SearchTableView(QWidget *parent = nullptr);

    void searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo);

    void searchDistant(ir::option::basic_opt_uptr &&filters, const RemoteInfoPtr &remoteInfo);

    void stopAllSearch();

    ~SearchTableView() override
    {
        _pool.stop();
    }

private:
    void addFile(const RcloneFilePtr &file);

    void showCustomContextMenu();

    void terminateSearch();

    auto searchInfoWidget(const QString &remoteName) -> QWidget *;

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

    void searchFinished();

    void searchStarted();

};