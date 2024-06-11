#include "SyncTableModel.hpp"

SyncTableModel::SyncTableModel(QObject *parent) : QAbstractTableModel(parent)
{
	_headers = {
					"Source", "État", "Destination", "Taille", "Temps restant", "Temps écoulé", "Vitesse",
					"Vitesse moyenne"
			};
}

int SyncTableModel::rowCount(const QModelIndex &parent) const { return _data.size(); }
int SyncTableModel::columnCount(const QModelIndex &parent) const { return _headers.size(); }

QVariant SyncTableModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) { return _data[index.row()]->operator[](index.column()); }
	return QVariant();
}

void SyncTableModel::clear()
{
	beginResetModel();
	_data.clear();
	endResetModel();
}

QVariant SyncTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) { return _headers[section]; }
	return QString::number(section + 1); // Afficher les numéros de ligne
}

void SyncTableModel::setData(const std::vector<SyncRow *> &rows)
{
	beginInsertRows(QModelIndex(), 0, rows.size() - 1);
	_data = rows;
	endInsertRows();
}

void SyncTableModel::updateRowData(int row)
{
	if (row < 0 || row >= rowCount())
	{
		return; // Vérifiez que la ligne est valide
	}

	// Notifiez la vue que les données ont changé
	QModelIndex topLeft = index(row, 0);
	QModelIndex bottomRight = index(row, columnCount() - 1);
	emit dataChanged(topLeft, bottomRight);
}
