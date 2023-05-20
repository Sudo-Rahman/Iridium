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
//	_back_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _back_button->setFixedSize(35, 35);
    btnLayout->addWidget(_back_button);

    _front_button = new RoundedButton(">", this);
//	_front_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _front_button->setFixedSize(35, 35);
    btnLayout->addWidget(_front_button);

    _refresh_button = new RoundedButton("↻", this);
//	_refresh_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _refresh_button->setFixedSize(35, 35);
    btnLayout->addWidget(_refresh_button);


    _path_label = new QLabel(this);
    _path_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _path_label->setAlignment(Qt::AlignLeft);
    _path_label->setContentsMargins(10, 0, 0, 0);
    // toolTip is used to show the full path
    _path_label->setMouseTracking(true);
    btnLayout->addWidget(_path_label);

    btnLayout->setAlignment(Qt::AlignLeft);
    btnLayout->setSpacing(0);

    _treeFileView = new TreeFileView(this);
    _layout->addWidget(_treeFileView);


    auto search_dialog = new QDialog(this);
    search_dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);


    _search_line_edit = new QLineEdit(this);
    auto hide = _search_line_edit->addAction(style()->standardIcon(QStyle::SP_LineEditClearButton),
                                             QLineEdit::LeadingPosition);
    _search_line_edit->setPlaceholderText("...");
    _search_line_edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(hide, &QAction::triggered, _search_line_edit, [search_dialog, this]()
    {
        _search_line_edit->clear();
        search_dialog->hide();
    });

    auto search_layout = new QHBoxLayout(search_dialog);
    search_layout->setSizeConstraint(QLayout::SetFixedSize);
    search_layout->setContentsMargins(0, 0, 0, 0);
    search_layout->addWidget(_search_line_edit);

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
    connect(_treeFileView, &TreeFileView::ctrlFPressed, this, [search_dialog, this]()
    {
        _search_line_edit->clear();
        _search_line_edit->setFixedWidth(_treeFileView->width() * .4);
        if (search_dialog->isHidden())
        {
            search_dialog->show();
            auto pos = _treeFileView->mapToGlobal(_treeFileView->rect().bottomRight() -
                                                  QPoint(_treeFileView->width() * .06,
                                                         _treeFileView->height() * .05));
            search_dialog->move(pos.x() - search_dialog->width(), pos.y() - search_dialog->height());
        }
    });
    connect(_search_line_edit, &QLineEdit::textChanged, this, [this](const QString &text)
    {
        _treeFileView->search(text);
        _treeFileView->setFocus();
    });
    connect(_treeFileView, &TreeFileView::pathChanged, this, [search_dialog, this]()
    {
        _search_line_edit->clear();
        search_dialog->hide();
    });
    connect(_treeFileView, &TreeFileView::resized, this, [search_dialog, this]()
    {
        _search_line_edit->setFixedWidth(_treeFileView->width() * .4);
        if (_treeFileView->size().width() < search_dialog->size().width() * 1.1)
            search_dialog->hide();
        auto pos = _treeFileView->mapToGlobal(_treeFileView->rect().bottomRight() -
                                              QPoint(_treeFileView->width() * .06,
                                                     _treeFileView->height() * .05));
        search_dialog->move(pos.x() - search_dialog->width(), pos.y() - search_dialog->height());
    });
}