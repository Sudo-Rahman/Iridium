//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEITEM_HPP
#define IRIDIUM_TREEFILEITEM_HPP

#include <QStandardItem>
#include <QJsonObject>
#include "../../Rclone/RcloneFile.hpp"

class TreeFileItem : public QStandardItem
{
protected:
	std::shared_ptr<RcloneFile> file{};
	TreeFileItem *parent{};

public:
	explicit TreeFileItem(TreeFileItem * = nullptr);

	explicit TreeFileItem(const QString& path, const RemoteInfoPtr  &remoteInfo, TreeFileItem * = nullptr);

	explicit TreeFileItem(QString path,const std::shared_ptr<RcloneFile> &file , TreeFileItem * = nullptr);

	[[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;

	[[nodiscard]] TreeFileItem *getParent() const;

protected:
	void initIcon();

};


#endif //IRIDIUM_TREEFILEITEM_HPP
