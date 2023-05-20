//
// Created by Rahman on 19/04/2023.
//

#ifndef IRIDIUM_RCLONEFRAME_HPP
#define IRIDIUM_RCLONEFRAME_HPP

#include <QFrame>
#include <QSpinBox>
#include <QComboBox>

class RcloneFrame : public QFrame
{
Q_OBJECT

    QSpinBox *_max_rclone_execution{}, *_parallel_transfers{}, *_max_depth{}, *_stats_refresh{};
    QComboBox *_load_type{};


public:
    explicit RcloneFrame(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_RCLONEFRAME_HPP
