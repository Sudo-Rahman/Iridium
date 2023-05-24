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
#include <QProxyStyle>
#include <memory>

// https://stackoverflow.com/questions/74929660/qtableview-how-to-hide-dotted-line-border-when-no-items-are-selected
class NoFocusOutlineStyle : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget) const override
    {
        if (element == QStyle::PE_FrameFocusRect)
            return;
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

SearchTableView::SearchTableView(QWidget *parent) : QTableView(parent)
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

    initThread();

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

    setStyle(new NoFocusOutlineStyle());
    setFrameStyle(QFrame::NoFrame);
    setShowGrid(false);
}

void SearchTableView::showCustomContextMenu()
{
    auto items = QTableView::selectedIndexes();
    if (items.empty())
        return;
    auto menu = QMenu(this);
    auto copie = menu.addAction(tr("Copier les fichiers"));
    copie->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    switch (items.size() / horizontalHeader()->count())
    {
        case 1:
        {
            auto parent = menu.addAction(tr("Copier le dossier parent"));
            parent->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
            copie->setText(tr("Copier le fichier"));
            connect(parent, &QAction::triggered, this, [this, items]()
            {
                auto item = dynamic_cast<SearchTableItem *>(_model->itemFromIndex(items[0]));
                Iridium::Global::clear_and_swap_copy_files(
                        {std::make_shared<RcloneFile>(item->getFile()->getParentDir())});
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
            auto item_cast = dynamic_cast<SearchTableItem *>(_model->itemFromIndex(item));
            files.push_back(item_cast->getFile());
        }
        Iridium::Global::clear_and_swap_copy_files(files);
        return;
    }
}

void SearchTableView::addFile()
{
    auto file = std::move(_rows.front().file);
    auto remoteInfo = _rows.front().remoteInfo;
    _rows.erase(_rows.begin());
    try
    {
        RcloneFilePtr rcloneFile = std::make_shared<RcloneFile>(
                (remoteInfo->path + std::string(file.at("Path").as_string())).c_str(),
                file.at("Size").as_int64(),
                file.at("IsDir").as_bool(),
                QDateTime::fromString(file.at("ModTime").as_string().c_str(), Qt::ISODate),
                remoteInfo
        );
        QList<QStandardItem *> row = {
                new SearchTableItem(0, rcloneFile),
                new SearchTableItem(1, rcloneFile),
                new SearchTableItem(2, rcloneFile),
                new SearchTableItem(3, rcloneFile),
                new SearchTableItem(4, rcloneFile),
                new SearchTableItem(5, rcloneFile)
        };
        _model->appendRow(row);
    } catch (const boost::exception &e)
    {
        std::cerr << "add file : " << file << std::endl;
        std::cerr << boost::diagnostic_information(e) << std::endl;
    }catch (...){
        std::cerr << "add file : " << file << std::endl;
    }
}


void SearchTableView::searchLocal(const QString &text, const RemoteInfoPtr &remoteInfo)
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
                    QDirIterator it(remoteInfo->path.c_str(),
                                    QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot,
                                    QDirIterator::Subdirectories);
                    while (it.hasNext())
                    {
                        boost::this_thread::interruption_point();
                        it.next();
                        if (it.fileName().contains(text, Qt::CaseInsensitive))
                        {
                            RcloneFilePtr rcloneFile = std::make_shared<RcloneFile>(
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
                } catch (boost::thread_interrupted &e) {}
            });
    _threads.push_back(std::move(th));
}

/**
 * @brief SearchTableView::searchDistant search distant files
 * @param filters
 * @param remoteInfo
 */
void SearchTableView::searchDistant(const std::vector<Rclone::Filter> &filters, const RemoteInfoPtr &remoteInfo)
{
    if (filters.empty())
        return;
    auto rclone = RcloneManager::get();
    _rclones.push_back(rclone);
    emit searchStarted();
    connect(rclone.get(), &Rclone::searchRefresh,
            [this, remoteInfo](boost::json::object file)
            {
                _rows.push_back({std::move(file), remoteInfo});
            });
    connect(rclone.get(), &Rclone::finished, this, [this]()
    {
        terminateSearch();
        erase_if(_rclones,[this](const RclonePtr &rclone)
        {
            return rclone->state() == Rclone::State::Finsished;
        });
    });
    rclone->search(filters, *remoteInfo);
    _searching++;
    _cv.notify_one();
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
    _rows.clear();
    _searching = 0;
    for (auto &thread: _threads)
    {
        thread.interrupt();
        thread.join();
    }
    for (auto &rclone: _rclones)
        rclone->kill();
    _rclones.clear();
    _threads.clear();
    emit searchFinished();
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

void SearchTableView::initThread()
{
    _adder = std::make_unique<boost::thread>(
            [this]()
            {
                while (not boost::this_thread::interruption_requested())
                {
                    try
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        _cv.wait(lock, [this]()
                        {
                            boost::this_thread::interruption_point();
                            return not _rows.empty() or _searching > 0;
                        });
                        if (_rows.empty())
                            continue;
                        addFile();
                        boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
                    } catch (boost::thread_interrupted &e) { break; }
                }
            });
}