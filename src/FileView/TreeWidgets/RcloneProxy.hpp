//
// Created by sr-71 on 08/03/2023.
//

#ifndef IRIDIUM_RCLONEPROXY_HPP
#define IRIDIUM_RCLONEPROXY_HPP

#include <QSortFilterProxyModel>

class RcloneProxy : public QSortFilterProxyModel
{
Q_OBJECT
public:
	explicit RcloneProxy(QObject *parent = nullptr);

protected:
	bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;


};


#endif //IRIDIUM_RCLONEPROXY_HPP
