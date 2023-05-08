//
// Created by sr-71 on 08/05/2023.
//
#include <QFormLayout>

#include "SearchWidget.hpp"
#include <Rclone.hpp>
#include <Settings.hpp>

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    m_progressBar = new ProgressBar(ProgressBar::Linear, this);
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_progressBar->setFixedHeight(10);
    m_progressBar->hide();
    m_progressBar->setRange(0, 0);

    m_remotes_comboBox = new QComboBox(this);
    m_remotes = Settings::getRemotes();
    for (const auto &remote: m_remotes)
        m_remotes_comboBox->addItem(QIcon(remote->m_icon.c_str()), remote->name().c_str());

    m_search = new QLineEdit(this);
    m_button = new QPushButton(tr("Rechercher"), this);
    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_search_view = new SearchTableView(this);

    auto *formLayout = new QFormLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow(tr("Chercher dans : "), m_remotes_comboBox);
    formLayout->addRow(tr("Nom du Fichier : "), m_search);


    m_layout->addLayout(formLayout);
    m_layout->addWidget(m_button);
    m_layout->addWidget(m_progressBar);
    m_layout->addWidget(m_search_view);


    connect(m_button, &QPushButton::clicked, this, [this]()
    {
        m_search_view->search(m_search->text(), m_remotes[m_remotes_comboBox->currentIndex()]);
        m_progressBar->show();
    });
    connect(m_search_view, &SearchTableView::searchFinished, this, [this]()
    {
        m_progressBar->hide();
    });
}
