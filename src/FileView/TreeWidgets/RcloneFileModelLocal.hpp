//
// Created by Rahman on 30/03/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODELLOCAL_HPP
#define IRIDIUM_RCLONEFILEMODELLOCAL_HPP

#include "RcloneFileModel.hpp"
#include <boost/thread.hpp>

class RcloneFileModelLocal : public RcloneFileModel
{
Q_OBJECT

    boost::shared_ptr<boost::thread> m_thread{};

public:
    explicit RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QTreeView *View);


public:
    void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

protected:

    void init() override;
};


#endif //IRIDIUM_RCLONEFILEMODELLOCAL_HPP
