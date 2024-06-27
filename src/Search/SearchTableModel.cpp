//
// Created by sr-71 on 12/06/2024.
//

#include "SearchTableModel.hpp"

SearchTableModel::SearchTableModel(std::vector<SearchRow *> *rows,QObject *parent) : QAbstractTableModel(parent)
{
	_headers = {tr("Nom"), tr("Remote"), tr("Chemin"), tr("Taille"), tr("Date de modification"), tr("Type")};
    _data = rows;
}

int SearchTableModel::rowCount(const QModelIndex &parent) const {
    return _data == nullptr ? 0 : static_cast<int>(_data->size());
}
int SearchTableModel::columnCount(const QModelIndex &parent) const { return _headers.size(); }

QVariant SearchTableModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) { return _data->operator[](index.row())->data(index.column()); }
	return QVariant();
}

void SearchTableModel::clear()
{
	beginResetModel();
	endResetModel();
}

QVariant SearchTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) { return _headers[section]; }
	return QString::number(section + 1);
}

void SearchTableModel::sort(int column, Qt::SortOrder order)
{
	std::sort(_data->begin(), _data->end(), [column, order](SearchRow *a, SearchRow *b)
	{
		return a->compare(column, order, *b);
	});
	emit layoutChanged();
}

auto SearchTableModel::data(const QModelIndex &index) const -> SearchRow * { return _data->operator[](index.row()); }
auto SearchTableModel::rowsAppened() -> void
{
	emit layoutChanged();
}
