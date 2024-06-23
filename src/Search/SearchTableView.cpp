//
// Created by sr-71 on 07/05/2023.
//

#include "SearchTableView.hpp"
#include <QHeaderView>
#include <QMenu>
#include <Global.hpp>
#include <QDirIterator>
#include <Settings.hpp>
#include <QHBoxLayout>
#include <QLabel>
#include <QCache>

#include "CircularProgressBar.hpp"
#include "IridiumApp.hpp"
class CustomSearchItemDelegate : public QStyledItemDelegate
{
public:
    CustomSearchItemDelegate(SearchTableModel *model, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), _model(model)
    {
        _iconCache.setMaxCost(1000);  // limit size of the cache
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {

    	painter->save();

        auto icon = getIconForIndex(index);
        if (!icon.isNull()) {
            QRect iconRect = option.rect;
            QSize size(static_cast<int>(option.rect.height() / 1.5), static_cast<int>(option.rect.height() / 1.5));
            int yOffset = (iconRect.height() - size.height()) / 2;
            QPoint topLeft(iconRect.left() + 5, iconRect.top() + yOffset);
            QRect targetRect(topLeft, size);

            icon.paint(painter, targetRect, Qt::AlignLeft | Qt::AlignVCenter);

            QRect textRect = option.rect;
            textRect.setLeft(targetRect.right() + 5);

            drawElidedText(painter, textRect, index.data(Qt::DisplayRole).toString(), option.font);
        } else {
            drawElidedText(painter, option.rect, index.data(Qt::DisplayRole).toString(), option.font);
        }

    	painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return QSize(size.width(), qMax(size.height(), 30));  // Hauteur minimale de 30 pixels
    }

private:
    SearchTableModel *_model;
    mutable QCache<std::pair<int, int>, QIcon> _iconCache;

    QIcon &getIconForIndex(const QModelIndex &index) const {
        std::pair key(index.row(), index.column());
        if (!_iconCache.contains(key)) {
            QIcon *icon = new QIcon;
            if (index.column() == 0)
                *icon = _model->data(index)->file()->getIcon();
            else if (index.column() == 1)
                *icon = _model->data(index)->file()->getRemoteInfo()->getIcon();
            _iconCache.insert(key, icon);
        }
        return *_iconCache[key];
    }

    void drawElidedText(QPainter *painter, const QRect &rect, const QString &text, const QFont &font) const {
        QFontMetrics fontMetrics(font);
        QString elidedText = fontMetrics.elidedText(text, Qt::ElideRight, rect.width());
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
    }
};

using namespace iridium::rclone;

SearchTableView::SearchTableView(QWidget *parent) : QTableView(parent)
{
	_model = new SearchTableModel(this);

	_model->setData(&_data);

	QTableView::setModel(_model);
	QTableView::setSelectionBehavior(QAbstractItemView::SelectRows);
	QTableView::setSelectionMode(QAbstractItemView::ContiguousSelection);
	QTableView::setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTableView::setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setMinimumSectionSize(120);
	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	setItemDelegate(new CustomSearchItemDelegate(_model,  this));

	setColumnWidth(0, 0);

	QTableView::setSortingEnabled(true);

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

	for (auto const &remote: Iridium::Global::remotes)
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
	copie->setIcon(QIcon(":/resources/copy.png"));
	switch (items.size() / horizontalHeader()->count())
	{
		case 1:
			{
				auto parent = menu.addAction(tr("Copier le dossier parent"));
				parent->setIcon(Settings::dirIcon());
				copie->setText(tr("Copier le fichier"));
				connect(parent, &QAction::triggered, this, [this, items]()
				{
					auto row = _model->data(items[0]);
					Iridium::Global::clear_and_swap_copy_files(
						{std::make_shared<RcloneFile>(*row->file()->parent())});
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
		for (auto &item: items)
		{
			auto row = _model->data(item);

			auto file_not_exist = std::ranges::find_if(files.begin(), files.end(),
			                                           [row](const RcloneFilePtr &file)
			                                           {
				                                           return file.get() == row->file().get();
			                                           }) == files.end();

			if (row and file_not_exist)
				files.push_back(row->file());
		}
		Iridium::Global::clear_and_swap_copy_files(files);
	}
}

void SearchTableView::addFile(const RcloneFilePtr &file)
{
	std::lock_guard lock(_mutex);
	_data.push_back(new SearchRow(file));
	if (_data.size() < 100)
		_model->rowsAppened();
	else if (((_data.size() - 100) / 100 - 1) == _refresh_model)
	{
		_model->rowsAppened();
		_refresh_model++;
	}
}

void SearchTableView::searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo)
{
	if (text.size() < 3)
		return;
	auto widget = searchInfoWidget(remoteInfo->name().c_str());
	widget->setParent(this);
	auto th = boost::thread(
		[this, text, remoteInfo, widget]
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
						auto root_file = &_remote_to_root_file[remoteInfo.get()];

						auto parent_info = QFileInfo(it.filePath());
						auto parent = std::make_shared<RcloneFile>(
								nullptr,
								parent_info.path(),
								parent_info.size(),
								true,
								parent_info.lastModified(),
								remoteInfo
							);

						root_file->add_child(parent);

						auto rcloneFile = std::make_shared<RcloneFile>(
								parent.get(),
								it.fileName(),
								it.fileInfo().size(),
								it.fileInfo().isDir(),
								it.fileInfo().lastModified(),
								remoteInfo
							);
						addFile(rcloneFile);
					}
				}
				terminateSearch();
				Iridium::Global::signal_remove_info(widget);
				std::erase(_search_info_widgets, widget);
			}
			catch (boost::thread_interrupted &e) {}
		});
	Iridium::Global::signal_add_info(widget);
	_search_info_widgets.push_back(widget);
	_threads.push_back(std::move(th));
}

/**
 * @brief SearchTableView::searchDistant search distant files
 * @param filters
 * @param remoteInfo
 */
void SearchTableView::searchDistant(option::basic_opt_uptr &&filters, const RemoteInfoPtr &remoteInfo)
{
	auto process = std::make_unique<ir::process>();
	auto widget = searchInfoWidget(remoteInfo->name().c_str());
	widget->setParent(this);
	process->add_option(std::move(filters));
	process->lsl(_remote_to_root_file[remoteInfo.get()]).every_line_parser(
				parser::file_parser::create(
					new parser::file_parser(&_remote_to_root_file[remoteInfo.get()],
					                        [this](const ire::file &file)
					                        {
						                        addFile(std::make_shared<RcloneFile>(file));
					                        },
					                        parser::file_parser::lsl)))
			.on_finish([widget,this](auto)
			{
				Iridium::Global::signal_remove_info(widget);
				std::erase(_search_info_widgets, widget);
				terminateSearch();
			})
			.on_start([widget,this]
			{
				Iridium::Global::signal_add_info(widget);
				_search_info_widgets.push_back(widget);
				_searching++;
				emit searchStarted();
			});
	_pool.add_process(std::move(process));
}

/**
 * @brief SearchTableView::terminateSearch when a search is finished call this function
 */
void SearchTableView::terminateSearch()
{
	if ((_searching--) == 1)
	{
		emit searchFinished();
		_model->rowsAppened();
	}
}

/**
 * @brief SearchTableView::stopAllSearch stop all search, local and distant
 */
void SearchTableView::stopAllSearch()
{
	for (auto &remote: _remote_to_root_file)
		remote.second.children().clear();
	_pool.stop_all_processes_and_clear();
	for (auto &th: _threads)
		th.interrupt();
	_threads.clear();
	_searching = 0;
	_model->clear();
	_data.clear();

	for (const auto &widget: _search_info_widgets)
		Iridium::Global::signal_remove_info(widget);
	_search_info_widgets.clear();
}

/**
 * @brief resize event
 * @param event
 */
void SearchTableView::resizeEvent(QResizeEvent *event)
{
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	QAbstractItemView::resizeEvent(event);
	if (horizontalHeader()->count() > 0)
		horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
	if (horizontalHeader()->sectionSize(0) < QWidget::width() * .4)
		setColumnWidth(0, QWidget::width() * .4);
}

auto SearchTableView::searchInfoWidget(const QString &remoteName) -> QWidget *
{
	auto infoWidget = new QWidget();
	auto layout = new QHBoxLayout(infoWidget);
	auto *progressBar = new CircularProgressBar(infoWidget);
	progressBar->setSize(20);
	progressBar->infinite();
	layout->addWidget(new QLabel("Recherche dans " + remoteName + "..."));
	infoWidget->findChild<QLabel *>()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	layout->addWidget(progressBar);
	return infoWidget;
}
