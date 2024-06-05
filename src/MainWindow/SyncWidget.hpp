//
// Created by sr-71 on 14/06/2023.
//

#ifndef IRIDIUM_SYNCWIDGET_HPP
#define IRIDIUM_SYNCWIDGET_HPP


#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <RoundedLineEdit.hpp>
#include <LinearProgressBar.hpp>
#include <SyncTableView.hpp>

#include "FilterGroupBox.hpp"

class SyncWidget : public QWidget
{
Q_OBJECT

    QVBoxLayout *_layout{};
    SyncTableView *_view{};
    QComboBox *_src_comboBox{}, *_dst_comboBox{}, *_types_sync_comboBox{};
    LinearProgressBar *_sync_progressBar{};
    QPushButton *_sync_button{}, *_stop{};
    FilterGroupBox *_filter_group_box{};
    QWidget *_info_widget{};

    enum State
    {
        None,
        Analysing,
        Analysed,
        Syncing,
        Synced,
    };

    State _state{None};

public:
    explicit SyncWidget(QWidget *parent = nullptr);

private:
    void connectSignals();

protected:
    bool event(QEvent *event) override;

};


#endif //IRIDIUM_SYNCWIDGET_HPP
