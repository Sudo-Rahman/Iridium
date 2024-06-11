//
// Created by sr-71 on 11/06/2024.
//

#include "SyncProgressBarDelegate.hpp"

#include <QPalette>
#include <QPainterPath>

void SyncProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
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

	QString percentageText = progress == -1 ? QString(tr("erreur")) : QString::number(progress) + "%";
	QFont font = painter->font();
	int fontSize = option.rect.height() / 3; // Dynamically adjust font size
	font.setPointSize(fontSize);
	font.setBold(true);
	painter->setFont(font);
	painter->setPen(QPalette().color(QPalette::WindowText));
	painter->drawText(rect, Qt::AlignCenter, percentageText);

	painter->restore();
}

QSize SyncProgressBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}

QColor SyncProgressBarDelegate::currentColor(int progress) const
{
	if (progress == -1) return QColor(251, 81, 88);
	if (progress == 100) return QColor(50, 192, 88);
	return QColor(0, 119, 190);
}
