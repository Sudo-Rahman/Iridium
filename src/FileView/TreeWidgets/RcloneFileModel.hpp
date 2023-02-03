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
	RclonesManager manager{std::thread::hardware_concurrency() * 10};
	QString path{};
	std::shared_ptr<std::thread> m_thread{};
	QModelIndex m_root_index{};

public:
	enum Type
	{
		Local, Distant
	};

	explicit RcloneFileModel(Type, const QString &path, QObject *parent = nullptr);


private:
	void initDistant();

	void addItemDistant(const QString &path, TreeFileItem *parent);

	void initLocal();

public:
	void addItemLocal(const QString &path, TreeFileItem *parent);

	const QModelIndex &getRootIndex() const;

};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
