#pragma once

#include <QGroupBox>
#include <QLayout>
#include <QListView>
#include <QPushButton>
#include <iridium/options.hpp>

class FilterSearchGroupBox : public QGroupBox
{
Q_OBJECT

    QListView *m_listView{};
    QPushButton *_add_include, *_add_exclude{}, *m_remove{}, *m_up{}, *m_down{}, *m_edit{};
    QVBoxLayout *_layout{};

    ir::option::filter *_filter{};


    enum FilterType
    {
        Include,
        Exclude
    };

    friend class FilterItem;

public:
    explicit FilterSearchGroupBox(QWidget *parent = nullptr);

    ~FilterSearchGroupBox() override
    {
        delete _filter;
    };

    [[nodiscard]] ir::option::vector getFilters();

private :
    void connectSignals();

};
