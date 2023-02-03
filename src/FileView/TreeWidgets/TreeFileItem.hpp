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
	TreeFileItem *parent{};

public:
	explicit TreeFileItem(QString path, QJsonObject data, TreeFileItem * = nullptr);

	explicit TreeFileItem(QString path, const std::shared_ptr<RcloneFile> & = nullptr, TreeFileItem * = nullptr);

	explicit TreeFileItem(const RcloneFile &, TreeFileItem * = nullptr);

	[[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;

	TreeFileItem *getParent() const;


};


#endif //IRIDIUM_TREEFILEITEM_HPP
