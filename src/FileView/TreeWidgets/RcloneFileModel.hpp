//
// Created by sr-71 on 02/02/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODEL_HPP
#define IRIDIUM_RCLONEFILEMODEL_HPP

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include "../../Rclone/Rclone.hpp"


class RcloneFileModel : public QStandardItemModel
{
Q_OBJECT

protected:
	RemoteInfoPtr m_remoteInfo{};
	QModelIndex m_root_index{};

	virtual void init() = 0;

	explicit RcloneFileModel(const RemoteInfoPtr &remoteInfo, QObject *parent = nullptr);

public:
	virtual void addItem(const RcloneFilePtr &file, TreeFileItem *parent) = 0;

	void transferTo(const RcloneFilePtr &file, TreeFileItem *parent);

	[[nodiscard]] const QModelIndex &getRootIndex() const;

	static QList<QStandardItem *> getItemList(TreeFileItem *item);

};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
