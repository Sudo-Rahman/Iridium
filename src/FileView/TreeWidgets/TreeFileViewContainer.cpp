//
// Created by Rahman on 03/04/2023.
//

#include "TreeFileViewContainer.hpp"

#include <QKeyEvent>

/**
 * @brief Construct a new Tree File View Container:: Tree File View Container object
 * @param remoteInfo
 * @param parent
 */
TreeFileViewContainer::TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent)
{
    initUI();
    _treeFileView->changeRemote(remoteInfo);
}

TreeFileViewContainer::TreeFileViewContainer(QWidget *parent)
{
    initUI();
}

void TreeFileViewContainer::initUI()
{
    _layout = new QVBoxLayout(this);
    setContentsMargins(0, 0, 0, 0);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    auto btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(btnLayout);

    _back_button = new RoundedButton("<", this);
    _back_button->setFixedSize(35, 35);
    btnLayout->addWidget(_back_button);

    _front_button = new RoundedButton(">", this);
    _front_button->setFixedSize(35, 35);
    btnLayout->addWidget(_front_button);

    _refresh_button = new RoundedButton("↻", this);
    _refresh_button->setFixedSize(35, 35);
    btnLayout->addWidget(_refresh_button);


    _path_label = new QLabel(this);
    _path_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _path_label->setAlignment(Qt::AlignLeft);
    _path_label->setContentsMargins(10, 0, 0, 0);
    _path_label->setMouseTracking(true);
    btnLayout->addWidget(_path_label);

    btnLayout->setAlignment(Qt::AlignLeft);
    btnLayout->setSpacing(0);

    _treeFileView = new TreeFileView(this);
    _layout->addWidget(_treeFileView);

    connect(_back_button, &QPushButton::clicked, _treeFileView, &TreeFileView::back);
    connect(_front_button, &QPushButton::clicked, _treeFileView, &TreeFileView::front);
    connect(_treeFileView, &TreeFileView::pathChanged, this, [this](const QString &path)
    {
        _path_label->setToolTip(path);
        QFontMetrics metrix(_path_label->font());
        int width = _path_label->width() - 10;
        auto clippedText = metrix.elidedText(path, Qt::ElideRight, width);
        _path_label->setText(clippedText);
    });

    connect(_refresh_button, &QPushButton::clicked, this, [this] { _treeFileView->reload(); });
}