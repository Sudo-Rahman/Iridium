//
// Created by sr-71 on 10/05/2023.
//

#ifndef IRIDIUM_FILTERSEARCHGROUPBOX_HPP
#define IRIDIUM_FILTERSEARCHGROUPBOX_HPP

#include <QGroupBox>
#include <QLayout>
#include <QListView>
#include <QPushButton>
#include <Rclone.hpp>

class FilterSearchGroupBox : public QGroupBox
{
Q_OBJECT

    QListView *m_listView{};
    QPushButton *_add_include, *_add_exclude{}, *m_remove{}, *m_up{}, *m_down{}, *m_edit{};
    QVBoxLayout *_layout{};

public:
    explicit FilterSearchGroupBox(QWidget *parent = nullptr);

    [[nodiscard]] std::vector<Rclone::Filter> getFilters();

private :
    void connectSignals();

};


#endif //IRIDIUM_FILTERSEARCHGROUPBOX_HPP
