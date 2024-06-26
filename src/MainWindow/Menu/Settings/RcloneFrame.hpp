//
// Created by Rahman on 19/04/2023.
//

#ifndef IRIDIUM_RCLONEFRAME_HPP
#define IRIDIUM_RCLONEFRAME_HPP

#include <QFrame>
#include <QSpinBox>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>

class RcloneFrame : public QFrame
{
Q_OBJECT

    QSpinBox *_max_rclone_execution{}, *_parallel_transfers{}, *_stats_refresh{};
    QComboBox *_load_type{};
    QLineEdit *_rclone_path{};
    QSlider *_max_depth{};
    QLabel *_depth_label{};


public:
    explicit RcloneFrame(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_RCLONEFRAME_HPP
