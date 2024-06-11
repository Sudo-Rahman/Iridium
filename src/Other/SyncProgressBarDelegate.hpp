#pragma once

#include <QPainter>
#include <QStyledItemDelegate>


class SyncProgressBarDelegate : public QStyledItemDelegate
{
public:
	SyncProgressBarDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	QColor currentColor(int progress) const;
};
