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
#include <ProgressBar.hpp>
#include <SyncTableView.hpp>

class SyncWidget : public QWidget
{
Q_OBJECT

    QVBoxLayout *_layout{};
    SyncTableView *_view{};
    QComboBox *_src_comboBox{}, *_dst_comboBox{};
    ProgressBar *_sync_progressBar{};
    QPushButton *_sync_button{}, *_stop{};

    enum State{
        None,
        Analyse,
        Sync
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
