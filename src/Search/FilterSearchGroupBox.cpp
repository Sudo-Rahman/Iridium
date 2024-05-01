//
// Created by sr-71 on 10/05/2023.
//

#include "FilterSearchGroupBox.hpp"
#include <QStringListModel>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItem>

using namespace ir;

class FilterItem : public QStandardItem
{
	std::string _filter{};
	FilterSearchGroupBox::FilterType _type;

public:
	explicit FilterItem(const FilterSearchGroupBox::FilterType& type, const QString& text) : QStandardItem(text),
		_type(type)
	{
		setFlags(flags() & ~Qt::ItemIsDropEnabled);
		switch (type)
		{
			case FilterSearchGroupBox::FilterType::Include:
				setIcon(QIcon(":/resources/add.png"));
				break;
			case FilterSearchGroupBox::FilterType::Exclude:
				setIcon(QIcon(":/resources/remove.png"));
				break;
		}
	}

	void setData(const QVariant& value, int role) override
	{
		QStandardItem::setData(value, role);
		if (_type == FilterSearchGroupBox::FilterType::Include)
			_filter = std::string("+ " + value.toString().toStdString());
		else
			_filter = std::string("- " + value.toString().toStdString());
	}

	[[nodiscard]] std::string getFilter() const { return _filter; }
};


FilterSearchGroupBox::FilterSearchGroupBox(QWidget * parent) : QGroupBox(parent)
{
	setTitle(tr("Filtres (non disponible pour la recherche locale)"));
	setCheckable(true);
	setChecked(false);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_layout = new QVBoxLayout(this);
	_layout->setSpacing(0);
	_layout->setContentsMargins(0, 0, 0, 0);

	m_listView = new QListView(this);
	auto model = new QStandardItemModel(this);
	m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_listView->setDropIndicatorShown(true);
	m_listView->setModel(model);

	m_listView->setMaximumHeight(100);

	_add_include = new QPushButton(tr("Inclure"), this);
	_add_exclude = new QPushButton(tr("Exclure"), this);
	m_remove = new QPushButton(tr("Retirer"), this);

	m_up = new QPushButton(QIcon(":/resources/arrow-up.png").pixmap(20, 20), "", this);
	m_edit = new QPushButton(QIcon(":/resources/edit.png").pixmap(20, 20), "", this);
	m_down = new QPushButton(QIcon(":/resources/arrow-down.png").pixmap(20, 20), "", this);


	m_up->setFixedWidth(30);
	m_edit->setFixedWidth(30);
	m_down->setFixedWidth(30);

	for (auto btn: {_add_include, _add_exclude, m_remove, m_up, m_down})
		btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto up_down_listView_layout = new QHBoxLayout();
	auto include_btn_layout = new QHBoxLayout();
	auto up_down_btn_layout = new QVBoxLayout();
	include_btn_layout->setContentsMargins(0, 0, 0, 0);
	up_down_btn_layout->setContentsMargins(0, 0, 0, 0);

	include_btn_layout->addWidget(_add_include);
	include_btn_layout->addWidget(_add_exclude);
	include_btn_layout->addWidget(m_remove);

	up_down_btn_layout->addWidget(m_up);
	up_down_btn_layout->addWidget(m_edit);
	up_down_btn_layout->addWidget(m_down);


	up_down_listView_layout->addLayout(up_down_btn_layout);
	up_down_listView_layout->addWidget(m_listView);

	_layout->addLayout(include_btn_layout);
	_layout->addLayout(up_down_listView_layout);

	connectSignals();
}

void FilterSearchGroupBox::connectSignals()
{
	auto func = [this](QStandardItem * item, QStandardItemModel * model)
	{
		m_listView->openPersistentEditor(model->index(0, 0));

		// check input
		auto editor = dynamic_cast<QLineEdit *>(m_listView->indexWidget(model->index(0, 0)));
		auto * validator = new QRegularExpressionValidator(QRegularExpression(R"([^\/:?"<>|]*)"));
		editor->setFocus();
		editor->setValidator(validator);

		connect(editor, &QLineEdit::editingFinished, this, [this, model, item, editor]
		{
			disconnect(editor, &QLineEdit::editingFinished, nullptr, nullptr);
			// if input is not valid
			if (editor->text().size() < 3 and not editor->text().contains(QRegularExpression(R"([*]{1})")))
			{
				auto index = model->index(0, 0);
				model->removeRow(index.row());
			}
			else
			{
				m_listView->closePersistentEditor(model->index(0, 0));
				item->setText(editor->text());
			}
		});
	};

	connect(_add_include, &QPushButton::clicked, this, [this, func]
	{
		auto item = new FilterItem(Include, "");
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		model->insertRow(0, item);
		func(item, model);
	});

	connect(_add_exclude, &QPushButton::clicked, this, [this, func]
	{
		auto item = new FilterItem(Exclude, "");
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		model->insertRow(0, item);
		func(item, model);
	});

	connect(m_remove, &QPushButton::clicked, this, [this]
	{
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		auto index = m_listView->currentIndex();
		model->removeRow(index.row());
	});

	connect(m_up, &QPushButton::clicked, this, [this]
	{
		if (m_listView->model()->rowCount() < 2)
			return;
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		auto index = m_listView->currentIndex();
		if (index.row() > 0)
		{
			// get item and insert it at index.row() - 1
			auto item = model->takeItem(index.row());
			model->removeRow(index.row());
			model->insertRow(index.row() - 1, item);
			m_listView->setCurrentIndex(model->index(index.row() - 1, 0));
		}
	});

	connect(m_down, &QPushButton::clicked, this, [this]
	{
		if (m_listView->model()->rowCount() < 2)
			return;
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		auto index = m_listView->currentIndex();
		if (index.row() < model->rowCount() - 1)
		{
			// get item and insert it at index.row() + 1
			auto item = model->takeItem(index.row());
			model->removeRow(index.row());
			model->insertRow(index.row() + 1, item);
			m_listView->setCurrentIndex(model->index(index.row() + 1, 0));
		}
	});

	connect(m_edit, &QPushButton::clicked, this, [func, this]
	{
		auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
		auto item = dynamic_cast<FilterItem *>(model->item(m_listView->currentIndex().row()));
		if (item)
			func(item, model);
	});
}

/**
 * @brief get filters from listview
 * @return filters
 */
option::basic_opt_uptr FilterSearchGroupBox::getFilters()
{
	auto filters = option::filter::filter_file::uptr();
	auto model = dynamic_cast<QStandardItemModel *>(m_listView->model());
	for (int i = 0; i < model->rowCount(); ++i)
	{
		auto item = dynamic_cast<FilterItem *>(model->item(i));
		if (item)
			filters->add_filter(item->getFilter());
	}
	return filters;
}
