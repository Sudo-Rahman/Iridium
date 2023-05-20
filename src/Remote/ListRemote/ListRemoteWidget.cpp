//
// Created by Rahman on 29/03/2023.
//

#include <QPropertyAnimation>
#include <QMessageBox>
#include "ListRemoteWidget.hpp"
#include <AddNewRemoteDialog.hpp>
#include <Settings.hpp>
#include <QPalette>

/**
 * @brief constructeur
 * @param parent
 */
ListRemoteWidget::ListRemoteWidget(QWidget *parent) : QScrollArea(parent)
{
    // background transparent
    auto pal = this->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    setPalette(pal);
    setWidgetResizable(true);
    auto *widget = new QWidget(this);
    setWidget(widget);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    _layout = new QVBoxLayout(widget);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    _layout->setSpacing(10);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    _layout->setContentsMargins(5, 10, 5, 10);

    m_expand = new QPushButton(this);
    m_expand->setCheckable(true);
    m_expand->setChecked(true);
    m_expand->setIcon(Settings::hardDriveIcon());
    m_expand->setFixedWidth(35);
    // rounded button
    _layout->addWidget(m_expand);

    connect(m_expand, &QPushButton::clicked, this, [this]() { expand(); });

    auto *toplayout = new QHBoxLayout;

    m_add = new RoundedButton("＋");
    m_add->setFixedSize(35, 35);
    connect(m_add, &QPushButton::clicked, this, [this]()
    {
        auto *addRemote = new AddNewRemoteDialog(this);
        connect(addRemote, &AddNewRemoteDialog::newRemoteAdded, this, [this]()
        {
            Settings::refreshRemotesList();
            emit remoteClicked(_remoteselected);
        });
        addRemote->setModal(true);
        addRemote->show();
    });
    toplayout->addWidget(m_add);

    m_recherche = new RoundedLineEdit(this);
    m_recherche->setPlaceholderText(tr("Rechercher"));
    connect(m_recherche, &QLineEdit::textChanged, this, &ListRemoteWidget::searchRemote);
    toplayout->addWidget(m_recherche);

    _layout->addLayout(toplayout);


    m_remoteLayout = new QVBoxLayout;
    _layout->addLayout(m_remoteLayout);

    _remoteselected = std::make_shared<remotes_selected>();

    Settings::refreshRemotesList();
    getAllRemote();

    // no border
    setFrameShape(QFrame::NoFrame);

    m_width = QScrollArea::sizeHint().width();

    Settings::list_remote_changed.connect([this] { getAllRemote(); });

}

/**
 * @brief Récupère tous les remotes
 */
void ListRemoteWidget::getAllRemote()
{
    // clear layout
    for (auto *remote: m_listRemote)
        _layout->removeWidget(remote);
    qDeleteAll(m_listRemote);
    m_listRemote.clear();
    _remoteselected->clear();

//     cration des remoteWidget
    auto remotes = Iridium::Variable::remotes;
    if (remotes.empty())
        return;
    auto it = remotes.begin();
    m_listRemote << new RemoteWidget(*it, false, this);
    it++;
    for (; it not_eq remotes.end(); ++it)
        m_listRemote << new RemoteWidget(*it, true, this);

    for (auto *remote: m_listRemote)
    {
        connect(remote, &RemoteWidget::clicked, this, [this](RemoteWidget *remoteWidget)
        {
            if (_remoteselected->first not_eq nullptr)
                _remoteselected->first->setSelectedText("");
            if (_remoteselected->second not_eq nullptr)
                _remoteselected->second->setSelectedText("");
            if (m_selected)
                _remoteselected->first = remoteWidget;
            else
                _remoteselected->second = remoteWidget;
            if (_remoteselected->first == _remoteselected->second)
                _remoteselected->first->setSelectedText("➀➁");
            else
            {
                if (_remoteselected->first not_eq nullptr)
                    _remoteselected->first->setSelectedText("➀");
                if (_remoteselected->second not_eq nullptr)
                    _remoteselected->second->setSelectedText("➁");
            }
            m_selected = !m_selected;
            emit remoteClicked(_remoteselected);
        });

        connect(remote, &RemoteWidget::deleted, this, [this](auto *remoteWidget)
        {
            m_listRemote.clear();
            for (int i = m_remoteLayout->count() - 1; i >= 0; --i)
            {
                auto *item = m_remoteLayout->itemAt(i);
                m_remoteLayout->removeItem(item);
                item->widget()->deleteLater();
            }
            Settings::refreshRemotesList();
        });

        m_remoteLayout->addWidget(remote);
        if (!m_listRemote.isEmpty())
        {
            _remoteselected->first = m_listRemote.first();
            _remoteselected->first->setSelectedText("➀");
        }
        if (m_listRemote.size() > 1)
        {
            _remoteselected->second = m_listRemote[1];
            _remoteselected->second->setSelectedText("➁");
        }
    }
    emit remoteClicked(_remoteselected);
}

/**
 * @brief Recherche un remote en fonction de son nom
 * @param name
 */
void ListRemoteWidget::searchRemote(const QString &name)
{
    for (auto *remote: m_listRemote)
    {
        if (QString::fromStdString(remote->remoteInfo()->name()).contains(name, Qt::CaseInsensitive))
            showAnimation(remote);
        else
            hideAnimation(remote);
    }
}

/**
 * @brief expand animation with expand button clicked
 */
void ListRemoteWidget::expand()
{
    auto animation = new QPropertyAnimation(this, "maximumWidth");
    animation->setDuration(500);
    animation->setStartValue(width());
    animation->setEndValue(m_isExpand ? 45 : m_width);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_isExpand = !m_isExpand;
    if (not m_isExpand)
        m_add->hide();
    if (m_isExpand)
    {
        for (auto wid: m_listRemote)
            showAnimation(wid);
        showAnimation(m_recherche);

    } else
    {
        for (auto wid: m_listRemote)
            hideAnimation(wid);
        hideAnimation(m_recherche);
    }
    connect(animation, &QPropertyAnimation::finished, this, [this]()
    {
        if (m_isExpand)
            m_add->show();
    });
}

/**
 * @brief set animation for show widget in parameter
 * @param widget
 */
void ListRemoteWidget::showAnimation(QWidget *widget) const
{
    auto animation = new QPropertyAnimation(widget, "maximumWidth");
    animation->setDuration(300);
    animation->setStartValue(0);
    animation->setEndValue(m_width);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, &QPropertyAnimation::finished, widget, [widget]() { widget->show(); });
}

/**
 * @brief set animation for hide widget in parameter
 * @param widget
 */
void ListRemoteWidget::hideAnimation(QWidget *widget) const
{
    auto animation = new QPropertyAnimation(widget, "maximumWidth");
    animation->setDuration(300);
    animation->setStartValue(widget->width());
    animation->setEndValue(0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, &QPropertyAnimation::finished, widget, [widget]() { widget->hide(); });
}