//
// Created by Rahman on 03/04/2023.
//

#include "TreeFileViewContainer.hpp"

/**
 * @brief Construct a new Tree File View Container:: Tree File View Container object
 * @param remoteInfo
 * @param parent
 */
TreeFileViewContainer::TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent)
{
    initUI();
    m_treeFileView->changeRemote(remoteInfo);
}

TreeFileViewContainer::TreeFileViewContainer(QWidget *parent)
{
    initUI();
}

void TreeFileViewContainer::initUI()
{
    m_layout = new QVBoxLayout(this);
    setContentsMargins(0, 0, 0, 0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    auto btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(btnLayout);

    m_back_button = new RoundedButton("<", this);
//	m_back_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_back_button->setFixedSize(35, 35);
    btnLayout->addWidget(m_back_button);

    m_front_button = new RoundedButton(">", this);
//	m_front_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_front_button->setFixedSize(35, 35);
    btnLayout->addWidget(m_front_button);

    m_refresh_button = new RoundedButton("↻", this);
//	m_refresh_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_refresh_button->setFixedSize(35, 35);
    btnLayout->addWidget(m_refresh_button);


    m_path_label = new QLabel(this);
    m_path_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_path_label->setAlignment(Qt::AlignLeft);
    m_path_label->setContentsMargins(10, 0, 0, 0);
    // toolTip is used to show the full path
    m_path_label->setMouseTracking(true);
    btnLayout->addWidget(m_path_label);

    btnLayout->setAlignment(Qt::AlignLeft);
    btnLayout->setSpacing(0);

    m_treeFileView = new TreeFileView(this);
    m_layout->addWidget(m_treeFileView);


    connect(m_back_button, &QPushButton::clicked, m_treeFileView, &TreeFileView::back);
    connect(m_front_button, &QPushButton::clicked, m_treeFileView, &TreeFileView::front);
    connect(m_treeFileView, &TreeFileView::pathChanged, this, [this](const QString &path)
    {
        m_path_label->setToolTip(path);
        QFontMetrics metrix(m_path_label->font());
        int width = m_path_label->width() - 10;
        auto clippedText = metrix.elidedText(path, Qt::ElideRight, width);
        m_path_label->setText(clippedText);
    });

    connect(m_refresh_button, &QPushButton::clicked, this, [this] { m_treeFileView->reload(); });
}
