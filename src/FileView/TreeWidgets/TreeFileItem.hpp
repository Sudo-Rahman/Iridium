//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEITEM_HPP
#define IRIDIUM_TREEFILEITEM_HPP

#include <QStandardItem>
#include "../../Rclone/RcloneFile.hpp"

class TreeFileItem : public QStandardItem
{
	std::shared_ptr<RcloneFile> file{};

public:
	explicit TreeFileItem(QString path, QJsonObject data);

	explicit TreeFileItem(QString path);
	explicit TreeFileItem(const RcloneFile &);

	[[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;


};


#endif //IRIDIUM_TREEFILEITEM_HPP
