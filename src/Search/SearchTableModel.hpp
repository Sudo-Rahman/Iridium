#pragma once

#include <QStyledItemDelegate>
#include <QVariant>
#include <QPainterPath>
#include <SearchRow.hpp>
#include <QPainter>

class SearchTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	SearchTableModel(std::vector<SearchRow *> *rows,QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role) const override;

	void clear();

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void sort(int column, Qt::SortOrder order) override;

	auto data(const QModelIndex &index) const -> SearchRow *;

	auto rowsAppened() -> void;

private:
	std::vector<SearchRow *> *_data;
	QStringList _headers;
};
