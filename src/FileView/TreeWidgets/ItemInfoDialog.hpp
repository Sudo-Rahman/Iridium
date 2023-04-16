//
// Created by sr-71 on 11/02/2023.
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
	QGridLayout *m_layout{};

	std::shared_ptr<RcloneFile> m_file{};
	QLabel *m_icon{}, *m_name{}, *m_size{}, *m_type{}, *m_path{}, *m_modTime{}, *m_objs{};

	ProgressBar *m_loading1{}, *m_loading2{};

	QTimer m_timer{};

	int row{};

	boost::shared_ptr<boost::thread> m_thread{};

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
