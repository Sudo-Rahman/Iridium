#pragma once

#include <QGroupBox>
#include <QLayout>
#include <QListView>
#include <QPushButton>
#include <iridium/options.hpp>

class FilterGroupBox : public QGroupBox
{
	Q_OBJECT

	QListView *m_listView{};
	QPushButton *_add_include, *_add_exclude{}, *m_remove{}, *m_up{}, *m_down{}, *m_edit{};
	QVBoxLayout *_layout{};

	enum FilterType
	{
		Include,
		Exclude
	};

	friend class FilterItem;

public:
	explicit FilterGroupBox(QWidget *parent = nullptr);

	explicit FilterGroupBox(const QString &title, QWidget *parent = nullptr);

	[[nodiscard]] ir::option::basic_opt_uptr getFilters();

private :
	void connectSignals();

signals:
	void filterAdded();

	void filterRemoved();
};
