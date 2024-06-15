#pragma once

#include <QList>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QPainterPath>
#include <SyncRow.hpp>
#include <QPainter>

class SyncTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	SyncTableModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role) const override;

	void clear();

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void setData(const std::vector<SyncRow *> &rows);

	void updateRowData();

	void sort(int column, Qt::SortOrder order) override;

private:
	std::vector<SyncRow *> _data;
	QStringList _headers;
};
