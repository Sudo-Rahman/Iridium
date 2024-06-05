//
// Created by sr-71 on 14/06/2023.
//

#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include <iridium/rclone.hpp>
#include <Sync.hpp>

#include "RcloneFile.hpp"
#include "SyncRow.hpp"


class SyncTableView : public QTableView
{
	Q_OBJECT

	QStandardItemModel *_model{};
	RcloneFilePtr _src{}, _dst{};
	std::map<std::string, std::unique_ptr<SyncRow>> _rows{};
	std::vector<std::string> _errors{};

	ir::process *_process{nullptr};

protected:
	void resizeEvent(QResizeEvent *event) override;

public:
	explicit SyncTableView(QWidget *parent = nullptr);

	void setFiles(const RcloneFilePtr &src, const RcloneFilePtr &dst);

	void analyse(SyncType type, const iro::basic_opt_uptr &filters);

	void clear();

	void sync(SyncType type, const iro::basic_opt_uptr &filters);

	void stop() const;

signals:
	void analyseStarted() const;

	void analyseFinished() const;

	void syncStarted() const;

	void syncFinished() const;

	void stopped() const;

	void progress(float value) const;

	void errorSync(QString error) const;

	void errorCheck(QString error) const;
};
