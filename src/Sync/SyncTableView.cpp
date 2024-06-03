//
// Created by sr-71 on 14/06/2023.
//

#include "SyncTableView.hpp"
#include <QHeaderView>
#include <Settings.hpp>
#include <QFontMetrics>
#include <QMessageBox>
#include <boost/process/io.hpp>

#include "IridiumApp.hpp"
#include "SyncRow.hpp"

using namespace std::chrono;

SyncTableView::SyncTableView(QWidget *parent) : QTableView(parent)
{
	_model = new QStandardItemModel(0, 3, this);

	_model->setHorizontalHeaderLabels(
		{tr("Source"), tr("État"), tr("Destination")});

	QTableView::setModel(_model);
	QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
	QTableView::setSelectionMode(QAbstractItemView::ContiguousSelection);
	QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setMinimumSectionSize(120);

	QTableView::setSortingEnabled(true);

	_model->setSortRole(Qt::UserRole + 1);

	connect(horizontalHeader(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize)
	{
		if (logicalIndex == 0 and newSize < QWidget::width() * .4)
			setColumnWidth(0, QWidget::width() * .4);
		// get size of all columns
		int size = 0;
		for (int i = 0; i < horizontalHeader()->count(); i++)
			size += horizontalHeader()->sectionSize(i);
		if (size < horizontalHeader()->size().width())
			setColumnWidth(horizontalHeader()->count() - 1, horizontalHeader()->size().width() - size +
			                                                horizontalHeader()->sectionSize(
				                                                horizontalHeader()->count() - 1));
	});
	setColumnWidth(0, 0);

	setFrameStyle(QFrame::NoFrame);
	setShowGrid(false);
}

void SyncTableView::setFiles(const RcloneFilePtr &src, const RcloneFilePtr &dst)
{
	_src = src;
	_dst = dst;
}

void SyncTableView::analyse(SyncType type, const iro::basic_opt_uptr &filter)
{
	_rows.clear();
	auto process = std::make_unique<ir::process>();

	if (type == Sync)
		process->add_option(iro::basic_option::uptr("--one-way"));

	process->add_option(filter->copy_uptr());

	auto parser = irp::json_log_parser::ptr([this](const ire::json_log &log)
	{
		if (log.level() == ire::json_log::log_level::error)
		{
			if (log.message().contains("'" + _src->name() + "'")) {}
			else if (log.message().contains("'" + _dst->name() + "'"))
			{
				IridiumApp::runOnMainThread([this, log = std::move(log)]
				{
					auto src_file = std::make_shared<RcloneFile>(_src.get(), log.object().c_str(), 0, false,
					                                             QDateTime(),
					                                             _src->getRemoteInfo());
					auto dst_file = std::make_shared<RcloneFile>(_dst.get(), log.object().c_str(), 0, false,
					                                             QDateTime(),
					                                             _dst->getRemoteInfo());
					auto hash = std::hash<std::string>{}(
						std::string(src_file->absolute_path() + dst_file->absolute_path()));
					std::cout <<std::string(src_file->absolute_path() + dst_file->absolute_path() )<< std::endl;
					auto row = std::make_unique<SyncRow>(src_file, dst_file);
					_model->appendRow(*row);
					setIndexWidget(row->progressBarIndex(), row->progressBar());
					_rows[std::to_string(hash)] = std::move(row);
				});
			}
		}
	});

	process->check(*_src, *_dst)
			.on_start([this] { emit analyseStarted(); })
			.every_line_parser(std::move(parser))
			.on_finish([this](auto exit)
			{
				if (exit not_eq 0 and _rows.empty())
					emit errorCheck();
				else
					emit analyseFinished();
			});
	_process = process.get();
	Iridium::Global::add_process(std::move(process));
}

void SyncTableView::clear()
{
	_model->clear();
	_rows.clear();
}

void SyncTableView::sync(SyncType type, const iro::basic_opt_uptr &filters)
{
	auto process = std::make_unique<ir::process>();
	process->add_option(filters->copy_uptr(), iro::logging::stats("200ms"), iro::logging::use_json_log(),
	                    iro::logging::verbose());

	if (type == Sync)
		process->sync(*_src, *_dst);
	else if (type == Bisync)
		process->bi_sync(*_src, *_dst);

	auto parser = irp::json_log_parser::ptr([this](auto log)
	{
		if (log.level() == ire::json_log::log_level::info)
		{
			if (log.get_stats() not_eq nullptr)
			{
				auto finished = std::accumulate(_rows.begin(), _rows.end(), 0,
				                                [](auto acc, const auto &row)
				                                {
					                                return acc + (row.second->state() == SyncRow::Finished ? 1 : 0);
				                                });
				emit progress(finished / static_cast<float>(_rows.size()));

				auto hashList = std::vector<std::string>{};
				for (const auto &transfer: log.get_stats()->transferring)
				{
					auto hash = std::hash<std::string>{}(std::string(
						transfer.src_fs.value_or("") + "/" + transfer.name +
						transfer.dst_fs.value_or("") + "/" + transfer.name));
					std::cout <<std::string(
						transfer.src_fs.value_or("") + "/" + transfer.name +
						transfer.dst_fs.value_or("") + "/" + transfer.name)<< std::endl;
					hashList.push_back(std::to_string(hash));
					IridiumApp::runOnMainThread([this,transfer = std::move(transfer), hash]
					{
						if(_rows.contains(std::to_string(hash)))
							_rows[std::to_string(hash)]->setTransferData(transfer);
					});
				}

				for (const auto &[hash, row]: _rows)
				{
					if (!std::ranges::any_of(hashList, [&hash](const auto &h) { return h == hash; }))
					{
						if (row->state() == SyncRow::Syncing)
							IridiumApp::runOnMainThread([this, hash] { _rows[hash]->finish(); });
					}
				}
			}
		}
	});

	process->every_line_parser(std::move(parser))
			.on_start([this] { emit syncStarted(); })
			.on_finish([this](auto exit)
			{
				if (exit == 0)
					emit syncFinished();
				else
					emit errorSync();
			});

	_process = process.get();
	Iridium::Global::add_process(std::move(process));
}

void SyncTableView::stop() const
{
	if(_process and _process->is_running())
		_process->stop();
}

/**
 * @brief resize event
 * @param event
 */
void SyncTableView::resizeEvent(QResizeEvent *event)
{
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
	if (horizontalHeader()->sectionSize(0) < QWidget::width() * .4)
		setColumnWidth(0, QWidget::width() * .4);
}

auto startEllipsizedText(const QString &text, int max_width, const QFontMetrics &fm) -> QString
{
	if (fm.horizontalAdvance(text) <= max_width)
		return text;

	// Vérifier si le texte dépasse la largeur du QLabel
	if (fm.horizontalAdvance(text) > max_width)
	{
		// Calculer la largeur des "..." qui seront ajoutés
		int ellipsisWidth = fm.horizontalAdvance("...");

		// Taille disponible pour le texte (en tenant compte des "...")
		int availableWidth = max_width - 2 * ellipsisWidth;

		// Tronquer le texte au milieu
		return fm.elidedText(text, Qt::ElideMiddle, availableWidth);
	}

	return text;
}
