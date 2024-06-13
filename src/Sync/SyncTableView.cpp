//
// Created by sr-71 on 14/06/2023.
//

#include "SyncTableView.hpp"
#include <QHeaderView>
#include <Settings.hpp>
#include <QFontMetrics>
#include <QMessageBox>
#include <thread>
#include <SyncProgressBarDelegate.hpp>
#include "IridiumApp.hpp"
#include "SyncRow.hpp"
#include "SyncTableModel.hpp"

class CustomSyncItemDelegate : public QStyledItemDelegate
{
public:
	CustomSyncItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		// Supposons que les données du modèle contiennent le chemin de l'icône
		QString path = index.data().toString();
		QIcon icon(RcloneFile::getIcon(path));
		if (!icon.isNull())
		{
			QRect iconRect = option.rect;

			QSize size(static_cast<int>(option.rect.height()/1.5), static_cast<int>(option.rect.height()/1.5));

			int yOffset = (iconRect.height() - size.height()) / 2;
			QPoint topLeft(iconRect.left() + 5, iconRect.top() + yOffset);
			QRect targetRect(topLeft, size);

			icon.paint(painter, targetRect, Qt::AlignLeft | Qt::AlignVCenter);

			QRect textRect = option.rect;
			textRect.setLeft(targetRect.right() + 5); // Laisser un espace de 5 pixels entre l'icône et le texte

			QString text = index.data(Qt::DisplayRole).toString();
			QFontMetrics fontMetrics(option.font);
			QString elidedText = fontMetrics.elidedText(text, Qt::ElideRight, textRect.width());

			painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
		} else {
			// Si pas d'icône, dessiner normalement avec l'effet ellipse pour le texte
			QString text = index.data(Qt::DisplayRole).toString();
			QFontMetrics fontMetrics(option.font);
			QString elidedText = fontMetrics.elidedText(text, Qt::ElideRight, option.rect.width());
			painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
		}
	}
};

using namespace std::chrono;

SyncTableView::SyncTableView(QWidget *parent) : QTableView(parent)
{
	_model = new SyncTableModel(this);

	QTableView::setModel(_model);
	QTableView::setSelectionMode(QAbstractItemView::NoSelection);
	QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setMinimumSectionSize(100);

	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	QTableView::setSortingEnabled(true);

	setItemDelegateForColumn(1, new SyncProgressBarDelegate(this));
	setItemDelegateForColumn(0, new CustomSyncItemDelegate(this));

	connect(horizontalHeader(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int oldSize, int newSize)
	{
		if (logicalIndex == 0 and newSize < QWidget::width() * .3)
			setColumnWidth(0, QWidget::width() * .3);
		// get size of all columns
		int size = 0;
		for (int i = 0; i < horizontalHeader()->count(); i++)
			size += horizontalHeader()->sectionSize(i);
		if (size < horizontalHeader()->size().width())
			setColumnWidth(horizontalHeader()->count() - 1, horizontalHeader()->size().width() - size +
			                                                horizontalHeader()->sectionSize(
				                                                horizontalHeader()->count() - 1));
	});

	setFrameStyle(QFrame::NoFrame);
	setShowGrid(false);
}

void SyncTableView::setFiles(const RcloneFilePtr &src, const RcloneFilePtr &dst)
{
	_src = src;
	_dst = dst;
}

void SyncTableView::analyse(SyncType type, const iro::basic_opt_uptr &filters)
{
	_rows.clear();
	auto process = std::make_unique<ir::process>();

	if (type == Sync)
		process->add_option(iro::basic_option::uptr("--one-way"));

	if (filters) process->add_option(filters->copy_uptr());

	auto parser = irp::json_log_parser::ptr([this](const ire::json_log &log)
	{
		if (log.level() == ire::json_log::log_level::error)
		{
			if (log.message().contains("'" + _src->name() + "'"))
			{
				auto src_file = _dst->absolute_path() + "/" + log.object();
				auto dst_file = _src->absolute_path() + "/" + log.object();
				auto hash = std::hash<std::string>{}(std::string(src_file + dst_file));
				auto row = new SyncRow(src_file, dst_file, _data.size());
				_data.push_back(row);
				_rows[std::to_string(hash)] = row;
			}
			else if (log.message().contains("'" + _dst->name() + "'"))
			{
				auto src_file = _src->absolute_path() + "/" + log.object();
				auto dst_file = _dst->absolute_path() + "/" + log.object();
				auto hash = std::hash<std::string>{}(std::string(src_file + dst_file));
				auto row = new SyncRow(src_file, dst_file, _data.size());
				_data.push_back(row);
				_rows[std::to_string(hash)] = row;
			}
		}
	});

	process->check(*_src, *_dst)
			.on_start([this] { emit analyseStarted(); })
			.every_line_parser(std::move(parser))
			.on_stop([this] { emit stopped(); })
			.on_finish([this](auto exit, auto *p)
			{
				_model->setData(_data);
				if (exit not_eq 0 and _rows.empty())
					emit errorCheck(p->get_error().empty()
						                ? "Error"
						                : QString::fromStdString(_process->get_error().back()));
				else
					emit analyseFinished();
				p->clean_data();
			});
	_process = process.get();
	Iridium::Global::add_process(std::move(process));
}

void SyncTableView::clear()
{
	_model->clear();
	for (auto &row: _data)
		delete row;
	_data.clear();
	_rows.clear();
	_errors.clear();
}

void SyncTableView::sync(SyncType type, const iro::basic_opt_uptr &filters)
{
	auto process = std::make_unique<ir::process>();

	if (filters) process->add_option(filters->copy_uptr());
	process->add_option(iro::logging::stats("200ms"), iro::logging::use_json_log(),
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
				auto hashList = std::vector<std::string>{};
				for (const auto &transfer: log.get_stats()->transferring)
				{
					auto hash = std::hash<std::string>{}(std::string(
						transfer.src_fs.value_or("") + "/" + transfer.name +
						transfer.dst_fs.value_or("") + "/" + transfer.name));
					hashList.push_back(std::to_string(hash));

					if (_rows.contains(std::to_string(hash)) and
					    std::ranges::none_of(_errors, [&hash](const auto &h) { return h == std::to_string(hash); })
						)
					{
						auto ref = _rows[std::to_string(hash)];
						ref->setTransferData(transfer);
						_model->updateRowData(ref->row());
					}
				}

				for (const auto &[hash, row]: _rows)
				{
					if (!std::ranges::any_of(hashList, [&hash](const auto &h) { return h == hash; }))
					{
						if (row->state() == SyncRow::Syncing)
						{
							auto ref = _rows[hash];
							ref->finish();
							_model->updateRowData(ref->row());
							progress_counter++;
							emit progress(progress_counter / static_cast<float>(_rows.size()));
						}
					}
				}
			}
		}
		else if (log.level() == ire::json_log::log_level::error)
		{
			auto hash = std::hash<std::string>{}(
				_src->absolute_path() + "/" + log.object() + _dst->absolute_path() + "/" + log.object());
			if (_rows.contains(std::to_string(hash)) and
			    std::ranges::none_of(_errors, [&hash](const auto &h) { return h == std::to_string(hash); })
				)
			{
				auto ref = _rows[std::to_string(hash)];
				ref->error(log.message());
				_model->updateRowData(ref->row());
				_errors.push_back(std::to_string(hash));
				progress_counter++;
				emit progress(progress_counter / static_cast<float>(_rows.size()));
			}
		}
	});

	process->every_line_parser(std::move(parser))
			.on_start([this] { emit syncStarted(); })
			.on_stop([this] { emit stopped(); })
			.on_finish([this](auto exit, auto *p)
			{
				if (exit == 0)
					emit syncFinished();
				else
					emit errorSync(p->get_error().empty()
						               ? "Error"
						               : QString::fromStdString(_process->get_error().back()));
				p->clean_data();
			});

	_process = process.get();
	Iridium::Global::add_process(std::move(process));
}

void SyncTableView::stop() const
{
	if (_process and _process->is_running())
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
