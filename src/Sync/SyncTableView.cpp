//
// Created by sr-71 on 14/06/2023.
//

#include "SyncTableView.hpp"
#include <QHeaderView>
#include <Settings.hpp>


SyncTableView::SyncTableView(QWidget *parent) : QTableView(parent)
{
    _model = new QStandardItemModel(0, 4, this);

    _model->setHorizontalHeaderLabels(
            {tr("Source"), tr("Filtre"), tr("Progression"), tr("Destination")});

    QTableView::setModel(_model);
    QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
    QTableView::setSelectionMode(QAbstractItemView::ContiguousSelection);
    QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
    QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setMinimumSectionSize(120);

    QTableView::setSortingEnabled(true);

    _model->setSortRole(Qt::UserRole + 1);

    connect(horizontalHeader(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize)
    {
        if (logicalIndex == 0 and newSize < QWidget::width() * .4)
            setColumnWidth(0, QWidget::width() * .4);
        // get size of all columns
        int size = 0;
        for (int i = 0; i < horizontalHeader()->count(); i++)
            size += horizontalHeader()->sectionSize(i);
        if (size < horizontalHeader()->size().width())
            setColumnWidth(horizontalHeader()->count() - 1, horizontalHeader()->size().width() - size +
                                                            horizontalHeader()->sectionSize(
                                                                    horizontalHeader()->count() - 1));
    });
    setColumnWidth(0, 0);

    setFrameStyle(QFrame::NoFrame);
    setShowGrid(false);
    if (Settings::getSystem().os not_eq Settings::MacOs)
        setStyleSheet("QTableView { outline:none; }\n"
                      "QTableView::item:selected:focus { background: palette(highlight); }\n"
                      "QTableView::item:!selected:focus { background:transparent; }");
}


void SyncTableView::analyse()
{
}