//
// Created by sr-71 on 08/05/2023.
//

#ifndef IRIDIUM_SEARCHWIDGET_HPP
#define IRIDIUM_SEARCHWIDGET_HPP

#include <QWidget>
#include <RoundedLineEdit.hpp>
#include <QPushButton>
#include <QLayout>
#include <QComboBox>
#include "Remote/Remote.h"
#include "Other/LinearProgressBar.hpp"
#include <FilterGroupBox.hpp>


#include "Search/SearchTableView.hpp"

class SearchWidget : public QWidget
{
Q_OBJECT

    LinearProgressBar *_progressBar{};
    std::vector<RemoteInfoPtr> _remotes{};
    std::map<RemoteInfoPtr, bool> _remotes_status{};
    QComboBox *_remotes_comboBox{};
    RoundedLineEdit *_search{};
    QPushButton *_start{}, *_stop;
    QVBoxLayout *_layout{};
    SearchTableView *_search_view{};
    FilterGroupBox *_filter_search{};

public:
    explicit SearchWidget(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;

private:
    void connectSignals();

    void fillRemotesComboBox();
};


#endif //IRIDIUM_SEARCHWIDGET_HPP
