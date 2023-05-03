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

    QSpinBox *m_max_rclone_execution{}, *m_parallel_transfers{}, *m_max_depth{}, *m_stats_refresh{};
    QComboBox *m_load_type{};


public:
    explicit RcloneFrame(QWidget *parent = nullptr);

private:
    void connectSignals();

};


#endif //IRIDIUM_RCLONEFRAME_HPP
