//
// Created by sr-71 on 07/05/2023.
//

#include "SearchTableView.hpp"
#include <QHeaderView>
#include <Rclone.hpp>
#include <SearchTableItem.hpp>
#include <QMenu>
#include <Global.hpp>
#include <QDirIterator>
#include <Settings.hpp>

#include "IridiumApp.hpp"

using namespace iridium::rclone;

SearchTableView::SearchTableView(QWidget * parent) : QTableView(parent)
{
	_model = new QStandardItemModel(0, 6, this);

	_model->setHorizontalHeaderLabels(
		{tr("Nom"), tr("Remote"), tr("Chemin"), tr("Taille"), tr("Date de modification"), tr("Type")});

	QTableView::setModel(_model);
	QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
	QTableView::setSelectionMode(QAbstractItemView::ContiguousSelection);
	QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setMinimumSectionSize(120);

	setColumnWidth(0, 0);

	QTableView::setSortingEnabled(true);

	_model->setSortRole(Qt::UserRole + 1);

	connect(this, &QTableView::customContextMenuRequested, this, &SearchTableView::showCustomContextMenu);

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

	setFrameStyle(QFrame::NoFrame);
	setShowGrid(false);
	if (Settings::getSystem().os not_eq Settings::MacOs)
		setStyleSheet("QTableView { outline:none; }\n"
			"QTableView::item:selected:focus { background: palette(highlight); }\n"
			"QTableView::item:!selected:focus { background:transparent; }");

	for (auto const& remote: Iridium::Global::remotes)
	{
		_remote_to_root_file[remote.get()] = RcloneFile(nullptr, "", -1, true, QDateTime::currentDateTime(), remote);
	}
}

void SearchTableView::showCustomContextMenu()
{
	auto items = QTableView::selectedIndexes();
	if (items.empty())
		return;
	auto menu = QMenu(this);
	auto copie = menu.addAction(tr("Copier les fichiers"));
	copie->setIcon(QIcon(":/ressources/copy.png"));
	switch (items.size() / horizontalHeader()->count())
	{
		case 1:
			{
				auto parent = menu.addAction(tr("Copier le dossier parent"));
				parent->setIcon(Settings::dirIcon());
				copie->setText(tr("Copier le fichier"));
				connect(parent, &QAction::triggered, this, [this, items]()
				{
					// auto item = dynamic_cast<SearchTableItem *>(_model->itemFromIndex(items[0]));
					// Iridium::Global::clear_and_swap_copy_files(
					//         {std::make_shared<RcloneFile>(item->getFile()->absolute_path().c_str())});
				});
			}
			break;
		default:
			break;
	}
	auto action = menu.exec(QCursor::pos());

	if (action == copie)
	{
		std::vector<RcloneFilePtr> files;
		for (auto& item: items)
		{
			// auto item_cast = dynamic_cast<SearchTableItem *>(_model->itemFromIndex(item));
			// files.push_back(item_cast->getFile());
		}
		Iridium::Global::clear_and_swap_copy_files(files);
	}
}

void SearchTableView::addFile(const RcloneFilePtr& file) const
{
		QList<QStandardItem *> row = {
						new SearchTableItem(0, file),
						new SearchTableItem(1, file),
						new SearchTableItem(2, file),
						new SearchTableItem(3, file),
						new SearchTableItem(4, file),
						new SearchTableItem(5, file)
				};
		_model->appendRow(row);
}


void SearchTableView::searchLocal(const QString& text, const RemoteInfoPtr& remoteInfo)
{
	if (text.size() < 3)
		return;
	auto th = boost::thread(
		[this, text, remoteInfo]
		{
			try
			{
				emit searchStarted();
				_searching++;
				QDirIterator it(remoteInfo->full_path().c_str(),
				                QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot,
				                QDirIterator::Subdirectories);
				while (it.hasNext())
				{
					boost::this_thread::interruption_point();
					it.next();
					if (it.fileName().contains(text, Qt::CaseInsensitive))
					{

						RcloneFilePtr rcloneFile = std::make_shared<RcloneFile>(
							nullptr,
							it.filePath(),
							it.fileInfo().size(),
							it.fileInfo().isDir(),
							it.fileInfo().lastModified(),
							remoteInfo
						);
						_model->appendRow({
									new SearchTableItem(0, rcloneFile),
									new SearchTableItem(1, rcloneFile),
									new SearchTableItem(2, rcloneFile),
									new SearchTableItem(3, rcloneFile),
									new SearchTableItem(4, rcloneFile),
									new SearchTableItem(5, rcloneFile)
							});
					}
					boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
				}
				terminateSearch();
			}
			catch (boost::thread_interrupted& e) {}
		});
	_threads.push_back(std::move(th));
}

/**
 * @brief SearchTableView::searchDistant search distant files
 * @param filters
 * @param remoteInfo
 */
void SearchTableView::searchDistant(const option::vector& filters, const RemoteInfoPtr& remoteInfo)
{
	auto process = std::make_unique<ir::process>();
	process->add_option(filters);
	process->lsl(_remote_to_root_file[remoteInfo.get()]).every_line_parser(
				parser::file_parser::create(
					new parser::file_parser(&_remote_to_root_file[remoteInfo.get()],
					                        [this](const ire::file& file)
					                        {
					                        	// thread safe
						                        addFile(std::make_shared<RcloneFile>(file));
					                        },
					                        parser::file_parser::lsl)))
			.on_finish([this](auto) { terminateSearch(); })
			.on_start([this]
			{
				_searching++;
				emit searchStarted();
			});
	for (auto &option : filters)
		std::cout << option << std::endl;
	_pool.add_process(std::move(process));
}

/**
 * @brief SearchTableView::terminateSearch when a search is finished call this function
 */
void SearchTableView::terminateSearch()
{
	if ((_searching--) == 1)
		emit searchFinished();
}

/**
 * @brief SearchTableView::stopAllSearch stop all search, local and distant
 */
void SearchTableView::stopAllSearch()
{
	_pool.stop_all_processes_and_clear();
	_rows.clear();
	_searching = 0;
}

/**
 * @brief resize event
 * @param event
 */
void SearchTableView::resizeEvent(QResizeEvent * event)
{
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
	if (horizontalHeader()->sectionSize(0) < QWidget::width() * .4)
		setColumnWidth(0, QWidget::width() * .4);
}
