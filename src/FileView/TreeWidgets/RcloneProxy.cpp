//
// Created by sr-71 on 08/03/2023.
//

#include "RcloneProxy.hpp"

RcloneProxy::RcloneProxy(QObject *parent) : QSortFilterProxyModel(parent)
{


}

bool RcloneProxy::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
//	return QSortFilterProxyModel::lessThan(source_left, source_right);
	return source_left.data().toInt() < source_right.data().toInt();
}
