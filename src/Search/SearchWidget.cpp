//
// Created by sr-71 on 08/05/2023.
//
#include <QFormLayout>

#include "SearchWidget.hpp"
#include <Settings.hpp>
#include <QPainter>
#include <QEvent>
#include <QStyledItemDelegate>

class CustomStyledItemDelegate : public QStyledItemDelegate
{

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const override
    {
        auto & refToNonConstOption = const_cast<QStyleOptionViewItem &>(option);
        refToNonConstOption.showDecorationSelected = false;
        //refToNonConstOption.state &= ~QStyle::State_HasFocus & ~QStyle::State_MouseOver;

        QStyledItemDelegate::paint(painter, refToNonConstOption, index);
    }
};

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    m_progressBar = new ProgressBar(ProgressBar::Linear, this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_progressBar->setFixedHeight(10);
    m_progressBar->hide();
    m_progressBar->setRange(0, 0);

    auto model = new QStandardItemModel(this);
    m_remotes_comboBox = new QComboBox(this);
    m_remotes_comboBox->setModel(model);
    m_remotes_comboBox->setItemDelegate(new CustomStyledItemDelegate());
    m_remotes = Settings::getRemotes();


    auto it = m_remotes.begin();
    auto* item = new QStandardItem(it->get()->name().c_str());
    item->setIcon(QIcon(it->get()->m_icon.c_str()));
    item->setFlags( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    item->setData(Qt::Checked, Qt::CheckStateRole);
    model->appendRow(item);
    it++;
    for (;it not_eq m_remotes.end(); it++)
    {
        auto* item = new QStandardItem(it->get()->name().c_str());
        item->setIcon(QIcon(it->get()->m_icon.c_str()));
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        item->setFlags( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        model->appendRow(item);
    }

    m_search = new QLineEdit(this);
    m_start = new QPushButton(tr("Rechercher"), this);
    m_start->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    // if enter is pressed, start search
    m_start->setDefault(true);
    connect(m_search, &QLineEdit::returnPressed, m_start, &QPushButton::click);
    m_stop = new QPushButton(tr("Arrêter"), this);
    m_stop->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_stop->hide();
    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_search_view = new SearchTableView(this);

    auto *formLayout = new QFormLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow(tr("Chercher dans : "), m_remotes_comboBox);
    formLayout->addRow(tr("Nom du Fichier : "), m_search);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(m_start);
    buttonLayout->addWidget(m_stop);



    m_layout->addLayout(formLayout);
    m_layout->addLayout(buttonLayout);
    m_layout->addWidget(m_progressBar);
    m_layout->addWidget(m_search_view);

    connect(m_stop, &QPushButton::clicked, this, [this]()
    {
        m_search_view->stopAllSearch();
        m_progressBar->hide();
        m_stop->hide();
    });

    connect(m_start, &QPushButton::clicked, this, [model,this]()
    {
        if(m_search->text().isEmpty())
            return;
        m_search_view->stopAllSearch();
        m_search_view->model()->removeRows(0, m_search_view->model()->rowCount());
        auto remote_count = 0;
        for(int i = 0; i < model->rowCount(); i++)
        {
            auto item = model->item(i);
            if(item->checkState() == Qt::Checked)
            {
                m_search_view->search(m_search->text(), m_remotes[i]);
                m_progressBar->show();
                remote_count++;
            }
        }
        if(remote_count > 0)
            m_stop->show();
    });
    connect(m_search_view, &SearchTableView::searchFinished, this, [this]()
    {
        m_progressBar->hide();
        m_stop->hide();
    });
}

bool SearchWidget::event(QEvent *event) {
    if(event->type() == QEvent::InputMethodQuery){
        m_remotes_comboBox->findChildren<QWidget*>().first()->hide();
    }
    return QWidget::event(event);
}