//
// Created by sr-71 on 03/04/2023.
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

	m_backButton = new RoundedButton("<", this);
//	m_backButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_backButton->setFixedSize(35, 35);
	btnLayout->addWidget(m_backButton);

	m_frontButton = new RoundedButton(">", this);
//	m_frontButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_frontButton->setFixedSize(35, 35);
	btnLayout->addWidget(m_frontButton);

	m_refreshButton = new RoundedButton("↻", this);
//	m_refreshButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_refreshButton->setFixedSize(35, 35);
	btnLayout->addWidget(m_refreshButton);


	m_pathLabel = new QLabel(this);
	m_pathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_pathLabel->setAlignment(Qt::AlignLeft);
	m_pathLabel->setContentsMargins(10, 0, 0, 0);
	// toolTip is used to show the full path
	m_pathLabel->setMouseTracking(true);
	btnLayout->addWidget(m_pathLabel);

	btnLayout->setAlignment(Qt::AlignLeft);
	btnLayout->setSpacing(0);

	m_treeFileView = new TreeFileView(this);
	m_layout->addWidget(m_treeFileView);


	connect(m_backButton, &QPushButton::clicked, m_treeFileView, &TreeFileView::back);
	connect(m_frontButton, &QPushButton::clicked, m_treeFileView, &TreeFileView::front);
	connect(m_treeFileView, &TreeFileView::pathChanged, this, [this](const QString &path)
	{
		m_pathLabel->setToolTip(path);
		QFontMetrics metrix(m_pathLabel->font());
		int width = m_pathLabel->width() - 10;
		auto clippedText = metrix.elidedText(path, Qt::ElideRight, width);
		m_pathLabel->setText(clippedText);
	});

	connect(m_refreshButton, &QPushButton::clicked, this, [this]{ m_treeFileView->reload();});
}
