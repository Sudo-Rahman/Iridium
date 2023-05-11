//
// Created by sr-71 on 10/05/2023.
//

#include "FilterSearchGroupBox.hpp"
#include <QStringListModel>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>

class FilterItem : public QStandardItem
{
private:
    Rclone::FilterType m_type;

public:
    explicit FilterItem(const Rclone::FilterType &type, const QString &text) : QStandardItem(text), m_type(type)
    {
        setFlags(flags() & ~Qt::ItemIsDropEnabled);
        switch (type)
        {
            case Rclone::FilterType::Include:
                setIcon(QIcon(":/ressources/add.png"));
                break;
            case Rclone::FilterType::Exclude:
                setIcon(QIcon(":/ressources/remove.png"));
                break;
        }
    }

    [[nodiscard]] Rclone::FilterType getType() const { return m_type; }

};


FilterSearchGroupBox::FilterSearchGroupBox(QWidget *parent) : QGroupBox(parent)
{
    setTitle(tr("Filtres (non disponible pour la recherche locale)"));
    setCheckable(true);
    setChecked(false);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_listView = new QListView(this);
    auto model = new QStandardItemModel(this);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    // todo: drag and drop internal works but transform FilterItem to QStandardItem
//    m_listView->setDragDropMode(QAbstractItemView::InternalMove);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setDropIndicatorShown(true);
    m_listView->setModel(model);

    m_listView->setMaximumHeight(100);

    m_add_include = new QPushButton(tr("Inclure"), this);
    m_add_exclude = new QPushButton(tr("Exclure"), this);
    m_remove = new QPushButton(tr("Retirer"), this);

    for (auto btn: {m_add_include, m_add_exclude, m_remove})
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto include_btn_layout = new QHBoxLayout();
    include_btn_layout->setContentsMargins(0, 0, 0, 0);

    include_btn_layout->addWidget(m_add_include);
    include_btn_layout->addWidget(m_add_exclude);
    include_btn_layout->addWidget(m_remove);


    m_layout->addLayout(include_btn_layout);
    m_layout->addWidget(m_listView);

    connectSignals();
}

void FilterSearchGroupBox::connectSignals()
{
    auto func = [this](QStandardItem *item)
    {
        auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
        model->insertRow(0, item);
        m_listView->openPersistentEditor(model->index(0, 0));
        // get qlineedit from item
        auto editor = dynamic_cast<QLineEdit *>(m_listView->indexWidget(model->index(0, 0)));
        auto *validator = new QRegularExpressionValidator(QRegularExpression(R"([^\/:?"<>|]*)"));
        editor->setFocus();
        editor->setValidator(validator);

        connect(editor, &QLineEdit::editingFinished, this, [this, model, item, editor]
        {
            if (editor->text().size() < 3 and not editor->text().contains(QRegularExpression(R"([*]{1})")))
            {
                auto index = model->index(0, 0);
                model->removeRow(index.row());
            } else
            {
                m_listView->closePersistentEditor(model->index(0, 0));
                item->setText(editor->text());
            }
        });
    };

    connect(m_add_include, &QPushButton::clicked, this, [func]
    {
        auto item = new FilterItem(Rclone::FilterType::Include, "");
        func(item);
    });

    connect(m_add_exclude, &QPushButton::clicked, this, [func]
    {
        auto item = new FilterItem(Rclone::FilterType::Exclude, "");
        func(item);
    });

    connect(m_remove, &QPushButton::clicked, this, [this]
    {
        auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
        auto index = m_listView->currentIndex();
        model->removeRow(index.row());
    });
}

std::vector<Rclone::Filter> FilterSearchGroupBox::getFilters()
{
    std::vector<Rclone::Filter> filters;
    auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
    for (int i = 0; i < model->rowCount(); ++i)
    {
        auto item = dynamic_cast<FilterItem *>(model->item(i));
        switch (item->getType())
        {
            case Rclone::Include:
                filters.push_back({Rclone::FilterType::Include, item->text().toStdString()});
                break;
            case Rclone::Exclude:
                filters.push_back({Rclone::FilterType::Exclude, item->text().toStdString()});
                break;
        }
    }
    return filters;
}
