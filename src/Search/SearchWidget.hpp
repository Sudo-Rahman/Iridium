//
// Created by sr-71 on 08/05/2023.
//

#ifndef IRIDIUM_SEARCHWIDGET_HPP
#define IRIDIUM_SEARCHWIDGET_HPP

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QComboBox>
#include <Remote.h>
#include <ProgressBar.hpp>


#include <SearchTableView.hpp>

class SearchWidget : public QWidget
{
Q_OBJECT

    ProgressBar *m_progressBar{};
    std::vector<RemoteInfoPtr> m_remotes{};
    QComboBox *m_remotes_comboBox{};
    QLineEdit *m_search{};
    QPushButton *m_start{}, *m_stop;
    QVBoxLayout *m_layout{};
    SearchTableView *m_search_view{};

public:
    explicit SearchWidget(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
};


#endif //IRIDIUM_SEARCHWIDGET_HPP
