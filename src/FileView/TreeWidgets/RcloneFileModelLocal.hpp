//
// Created by sr-71 on 30/03/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODELLOCAL_HPP
#define IRIDIUM_RCLONEFILEMODELLOCAL_HPP

#include "RcloneFileModel.hpp"

class RcloneFileModelLocal : public RcloneFileModel
{
Q_OBJECT

public:
	explicit RcloneFileModelLocal(const RemoteInfo &remoteInfo, QObject *parent = nullptr);


public:
	void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

protected:
	void init() override;
};


#endif //IRIDIUM_RCLONEFILEMODELLOCAL_HPP
