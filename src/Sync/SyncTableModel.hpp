#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QPainterPath>
#include <QPalette>
#include <QPainter>

class SyncTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	SyncTableModel(QObject *parent = nullptr)
		: QAbstractTableModel(parent) {}

	int rowCount(const QModelIndex &parent = QModelIndex()) const override { return _data ? _data->size() : 0; }

	int columnCount(const QModelIndex &parent = QModelIndex()) const override { return _headers.size(); }

	QVariant data(const QModelIndex &index, int role) const override
	{
		if (role == Qt::DisplayRole) { return _data->operator[](index.row())->operator[](index.column()); }
		return QVariant();
	}

	void clear()
	{
		beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
		endRemoveRows();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role == Qt::DisplayRole && orientation == Qt::Horizontal) { return _headers[section]; }
		return QVariant();
	}

	void setData(std::vector<SyncRow *> *rows)
	{
		beginInsertRows(QModelIndex(), 0, rows->size() - 1);
		_data = rows;
		endInsertRows();
	}

	void updateRowData(int row)
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

private:
	std::vector<SyncRow *> *_data;
	QStringList _headers = {
					"Source", "État", "Destination", "Taille", "Temps restant", "Temps écoulé", "Vitesse",
					"Vitesse moyenne"
			};
};

class SyncProgressBarDelegate : public QStyledItemDelegate
{
public:
	SyncProgressBarDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		int progress = index.data().toInt();

		// Dessiner le fond de la barre de progression
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing);

		QRect rect = option.rect.marginsRemoved(QMargins(1, 5, 1, 5));
		int radius = rect.height() / 2;
		int progressWidth = progress == -1 ? rect.width() : rect.width() * progress / 100;

		// Draw background
		painter->setBrush(QPalette().color(QPalette::Mid));
		painter->setPen(Qt::NoPen);
		painter->drawRoundedRect(rect, radius, radius);

		// Set up clipping region to ensure the progress bar respects the rounded corners
		QPainterPath clipPath;
		clipPath.addRoundedRect(rect, radius, radius);
		painter->setClipPath(clipPath);

		// Draw progress bar
		QRect progressRect(rect.x(), rect.y(), progressWidth, rect.height());
		painter->setBrush(currentColor(progress));

		painter->drawRoundedRect(progressRect, radius, radius);

		// Dessiner le texte de progression

		QString percentageText = QString::number(progress) + "%";
		QFont font = painter->font();
		int fontSize = option.rect.height() / 3; // Dynamically adjust font size
		font.setPointSize(fontSize);
		font.setBold(true);
		painter->setFont(font);
		painter->setPen(QPalette().color(QPalette::WindowText));
		painter->drawText(rect, Qt::AlignCenter, percentageText);

		painter->restore();
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		return QStyledItemDelegate::sizeHint(option, index);
	}

private:
	QColor currentColor(int progress) const
	{
		if (progress == -1) return QColor(251, 81, 88);
		if (progress == 100) return QColor(50, 192, 88);
		return QColor(0, 119, 190);
	}
};
