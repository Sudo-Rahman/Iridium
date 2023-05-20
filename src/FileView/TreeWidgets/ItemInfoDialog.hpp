//
// Created by Rahman on 11/02/2023.
//

#ifndef IRIDIUM_ITEMINFODIALOG_HPP
#define IRIDIUM_ITEMINFODIALOG_HPP

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <boost/thread.hpp>
#include <Utility/Utility.hpp>
#include <QTimer>
#include "TreeFileItem.hpp"
#include <Rclone.hpp>
#include <ProgressBar.hpp>

class ItemInfoDialog : public QDialog
{
Q_OBJECT

private:
    QGridLayout *_layout{};

    std::shared_ptr<RcloneFile> _file{};
    TreeFileItem *_item{};
    QLabel *_icon{}, *_name{}, *_size{}, *_type{}, *_path{}, *_mod_time{}, *_objs{};

    ProgressBar *_loading1{}, *_loading2{};

    QTimer _timer{};

    int _row{};

    boost::shared_ptr<boost::thread> _thread{};

public:
    explicit ItemInfoDialog(TreeFileItem *item, QWidget *parent = nullptr);

private:
    void initSize();

    void initType();

    void initLabel();

protected:

    ~ItemInfoDialog() override;

signals:

    void m_threadFinished();

};


#endif //IRIDIUM_ITEMINFODIALOG_HPP
