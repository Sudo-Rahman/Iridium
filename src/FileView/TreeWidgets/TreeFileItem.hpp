//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEITEM_HPP
#define IRIDIUM_TREEFILEITEM_HPP

#include <QStandardItem>
#include <QJsonObject>
#include <RcloneFile.hpp>

class TreeFileItem : public QStandardItem
{
protected:
	RcloneFilePtr file{};
	TreeFileItem *parent{};
	QList<TreeFileItem *> children{};

public:
	explicit TreeFileItem(TreeFileItem * = nullptr);

	explicit TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem * = nullptr);

	explicit TreeFileItem(const QString &path, const std::shared_ptr<RcloneFile> &file, TreeFileItem * = nullptr,
						  bool initIco = false);

	[[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;

	[[nodiscard]] TreeFileItem *getParent() const;

	[[nodiscard]] const QList<TreeFileItem *> &getChildren() const
	{ return children; }

	void addChild(TreeFileItem *child)
	{ children << child; }

protected:

	void initIcon();

};


#endif //IRIDIUM_TREEFILEITEM_HPP
