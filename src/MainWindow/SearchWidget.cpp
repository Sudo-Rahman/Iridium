//
// Created by sr-71 on 08/05/2023.
//
#include <QFormLayout>

#include "SearchWidget.hpp"
#include "Config/Settings.hpp"
#include <QPainter>
#include <QEvent>
#include <QStyledItemDelegate>
#include <QListView>

class CustomStyledItemDelegate : public QStyledItemDelegate
{

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        auto &refToNonConstOption = const_cast<QStyleOptionViewItem &>(option);
        refToNonConstOption.showDecorationSelected = false;
        //refToNonConstOption.state &= ~QStyle::State_HasFocus & ~QStyle::State_MouseOver;

        QStyledItemDelegate::paint(painter, refToNonConstOption, index);
    }
};

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    m_filter_search = new FilterSearchGroupBox(this);

    m_progressBar = new ProgressBar(ProgressBar::Linear, this);
    setFocusPolicy(Qt::StrongFocus);
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_progressBar->setFixedHeight(10);
    m_progressBar->hide();
    m_progressBar->setRange(0, 0);

    auto model = new QStandardItemModel(this);
    m_remotes_comboBox = new QComboBox(this);
    m_remotes_comboBox->setModel(model);
    m_remotes_comboBox->setItemDelegate(new CustomStyledItemDelegate());
    // set 5 items visible
    auto view = new QListView();
    m_remotes_comboBox->setView(view);
    view->setMinimumWidth(150);
    m_remotes_comboBox->setMaxVisibleItems(3);

    m_remotes = Iridium::Variable::remotes;
    fillRemotesComboBox();

    Settings::list_remote_changed.connect(
            [this]()
            {
                m_remotes = Iridium::Variable::remotes;
                fillRemotesComboBox();
            });

    m_search = new QLineEdit(this);
    m_start = new QPushButton(tr("Rechercher"), this);
    m_start->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *validator = new QRegularExpressionValidator(QRegularExpression(R"([^\/:*?"<>|]*)"));
    m_search->setValidator(validator);
    // if enter is pressed, start search
    m_start->setDefault(true);
    connect(m_search, &QLineEdit::returnPressed, m_start, &QPushButton::click);
    m_stop = new QPushButton(tr("Arrêter"), this);
    m_stop->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_stop->hide();

    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(5, 5, 5, 5);
    auto top_layout = new QHBoxLayout;
    auto left_layout = new QVBoxLayout;

    m_search_view = new SearchTableView(this);

    auto *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignCenter);
    formLayout->setAlignment(Qt::AlignCenter);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow(tr("Chercher dans : "), m_remotes_comboBox);
    formLayout->addRow(tr("Nom du Fichier : "), m_search);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(m_start);
    buttonLayout->addWidget(m_stop);

    left_layout->addLayout(formLayout);
    left_layout->setAlignment(Qt::AlignCenter);
    left_layout->addLayout(buttonLayout);
    left_layout->addWidget(m_progressBar);

    top_layout->addLayout(left_layout);
    top_layout->addWidget(m_filter_search);
    m_layout->addLayout(top_layout);
    m_layout->addWidget(m_search_view);

    connectSignals();
}

void SearchWidget::connectSignals()
{

    connect(m_filter_search, &FilterSearchGroupBox::clicked, this, [this](const bool &checked)
    {
        m_search->setEnabled(not checked);
    });

    connect(m_stop, &QPushButton::clicked, this, [this]()
    {
        m_search_view->stopAllSearch();
    });

    connect(m_start, &QPushButton::clicked, this, [this]()
    {
        auto model = dynamic_cast<QStandardItemModel *>(m_remotes_comboBox->model());
        m_search_view->stopAllSearch();
        m_search_view->model()->removeRows(0, m_search_view->model()->rowCount());
        for (int i = 0; i < model->rowCount(); i++)
        {
            auto item = model->item(i);
            if (item->checkState() == Qt::Checked)
            {
                if (m_filter_search->isChecked() and not m_remotes[i]->isLocal())
                    m_search_view->searchDistant(m_filter_search->getFilters(), m_remotes[i]);
                else
                {
                    if (m_remotes[i]->isLocal())
                        m_search_view->searchLocal(m_search->text(), m_remotes[i]);
                    else
                    {
                        m_search_view->searchDistant(
                                {{Rclone::Include, "*" + m_search->text().toStdString() + "*"},
                                 {Rclone::Exclude, "*"}},
                                m_remotes[i]);
                    }
                }
            }
        }
    });

    connect(m_search_view, &SearchTableView::searchStarted, this, [this]()
    {
        m_progressBar->show();
        m_stop->show();
    });

    connect(m_search_view, &SearchTableView::searchFinished, this, [this]()
    {
        m_progressBar->hide();
        m_stop->hide();
    });
}

bool SearchWidget::event(QEvent *event)
{
    if (event->type() == QEvent::InputMethodQuery)
    {
        m_remotes_comboBox->findChildren<QWidget *>().first()->hide();
    }
    return QWidget::event(event);
}

void SearchWidget::fillRemotesComboBox() {
    auto model = dynamic_cast<QStandardItemModel *>(m_remotes_comboBox->model());
    model->clear();
    auto it = m_remotes.begin();
    auto *item = new QStandardItem(it->get()->name().c_str());
    item->setIcon(QIcon(it->get()->m_icon.c_str()));
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Checked, Qt::CheckStateRole);
    model->appendRow(item);
    it++;
    for (; it not_eq m_remotes.end(); it++)
    {
        auto *item = new QStandardItem(it->get()->name().c_str());
        item->setToolTip(it->get()->name().c_str());
        item->setIcon(QIcon(it->get()->m_icon.c_str()));
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        model->appendRow(item);
    }
}