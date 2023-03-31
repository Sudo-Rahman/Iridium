//
// Created by sr-71 on 02/02/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODEL_HPP
#define IRIDIUM_RCLONEFILEMODEL_HPP

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include "../../Rclone/Rclone.hpp"
#include <thread>


class RcloneFileModel : public QStandardItemModel
{
Q_OBJECT

protected:
	RemoteInfo m_remoteInfo{};
	std::shared_ptr<std::thread> m_thread{};
	QModelIndex m_root_index{};


	virtual void init() = 0;

	explicit RcloneFileModel(const RemoteInfo &remoteInfo, QObject *parent = nullptr);

public:
	virtual void addItem(const QString &path, TreeFileItem *parent) = 0;

	[[nodiscard]] const QModelIndex &getRootIndex() const;

	static QList<QStandardItem *> getItemList(TreeFileItem *item);

};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
