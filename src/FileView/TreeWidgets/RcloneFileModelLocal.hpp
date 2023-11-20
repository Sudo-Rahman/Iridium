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

    std::unique_ptr<boost::thread> _thread{};

public:
    explicit RcloneFileModelLocal(const RemoteInfoPtr &remoteInfo, QTreeView *View);


public:
    void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

    void stop() override {};

protected:

    void init() override;

    void reload(TreeFileItem *item) override;
};


#endif //IRIDIUM_RCLONEFILEMODELLOCAL_HPP
