//
// Created by sr-71 on 29/05/2024.
//

#pragma once
#include <LinearProgressBar.hpp>
#include <QStandardItem>
#include <RcloneFile.hpp>

class SyncRow : public QList<QStandardItem *>
{

public:
	explicit SyncRow(const RcloneFilePtr &file_src, const RcloneFilePtr &file_dst);

	QModelIndex progressBarIndex() const;

	QWidget *progressBar() const;

	RcloneFilePtr src() const;

	RcloneFilePtr dst() const;

	void setTransferData(const ire::json_log::stats::transfer &transfer);

	void finish();

	void error(const std::string &message);

	enum State
	{
		None,
		Syncing,
		Finished,
		Error
	};

	State state() const;

private:

	LinearProgressBar *_progressBar;
	QWidget *_widget;
	RcloneFilePtr _src;
	RcloneFilePtr _dst;
	State _state{None};

	QTimer _elapsed_time{};
	uint64_t _elapsed_time_count{};

};
