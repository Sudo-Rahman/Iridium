//
// Created by sr-71 on 03/04/2023.
//

#include "TreeFileViewContainer.hpp"

TreeFileViewContainer::TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent)
{
	m_layout = new QVBoxLayout(this);
	setContentsMargins(0, 0, 0, 0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);

	auto btnLayout = new QHBoxLayout();
	btnLayout->setContentsMargins(0, 0, 0, 0);
	m_layout->addLayout(btnLayout);

	m_backButton = new QPushButton("<", this);
	m_backButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	btnLayout->addWidget(m_backButton);

	m_frontButton = new QPushButton(">", this);
	m_frontButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	btnLayout->addWidget(m_frontButton);

	m_pathLabel = new QLabel(this);
	m_pathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_pathLabel->setAlignment(Qt::AlignLeft);
	// toolTip is used to show the full path
	m_pathLabel->setMouseTracking(true);
	btnLayout->addWidget(m_pathLabel);

	btnLayout->setAlignment(Qt::AlignLeft);
	btnLayout->setSpacing(10);

	m_treeFileView = new TreeFileView(remoteInfo, this);
	m_layout->addWidget(m_treeFileView);


	connect(m_backButton, &QPushButton::clicked, m_treeFileView, &TreeFileView::back);
	connect(m_frontButton, &QPushButton::clicked, m_treeFileView, &TreeFileView::front);
	connect(m_treeFileView, &TreeFileView::pathChanged, this, [=](const QString &path)
	{
		m_pathLabel->setToolTip(path);
		QFontMetrics metrix(m_pathLabel->font());
		int width = m_pathLabel->width() - 5;
		auto clippedText = metrix.elidedText(path, Qt::ElideRight, width);
		m_pathLabel->setText(clippedText);
	});

	connect(m_treeFileView, &TreeFileView::fileCopied, this, [this](TreeFileItem *item)
	{emit fileCopied(item);});

}

void TreeFileViewContainer::changeRemote(const RemoteInfoPtr &remoteInfo)
{
	m_treeFileView->changeRemote(remoteInfo);
}