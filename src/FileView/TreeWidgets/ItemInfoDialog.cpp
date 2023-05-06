//
// Created by Rahman on 11/02/2023.
//

#include "ItemInfoDialog.hpp"
#include <Utility/Utility.hpp>
#include <QDirIterator>
#include <QPropertyAnimation>
#include <QProgressBar>

using namespace Iridium;

ItemInfoDialog::ItemInfoDialog(TreeFileItem *item, QWidget *parent) : QDialog(parent), m_item(item)
{
    // delete on close
    setAttribute(Qt::WA_DeleteOnClose);
    m_file = item->getFile();
    setMaximumSize(500, 600);
    setMinimumWidth(300);
    setWindowTitle(tr("Information"));
    setModal(false);
    m_layout = new QGridLayout(this);
    m_layout->setColumnStretch(1, 1);
    m_layout->setVerticalSpacing(20);
    m_layout->setAlignment(Qt::AlignTop);


    initLabel();

    m_icon->setPixmap(item->icon().pixmap(128, 128));
    m_layout->addWidget(m_icon, row, 0, 2, 2);
    row += 2;

    initType();

    m_layout->addWidget(new QLabel(tr("Chemin Absolu: ")), row, 0, 1, 1);
    m_layout->addWidget(m_path, row, 1, 1, 1);
    row++;

    m_layout->addWidget(new QLabel(tr("Nom: ")), row, 0, 1, 1);
    m_layout->addWidget(m_name, row, 1, 1, 1);
    row++;

    if (m_item->index().model()->index(0, 0) == m_item->index())
    {
        m_name->setText(m_file->getRemoteInfo()->name().c_str());
        auto rclone = RcloneManager::get();
        m_mod_time->hide();
        m_loading1 = new ProgressBar(ProgressBar::Circular, this);
        m_loading1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_loading1->setFixedSize(100, 100);
        m_loading1->setRange(0, 0);
        m_layout->addWidget(m_loading1, row, 0, 1, 3, Qt::AlignCenter);
        connect(rclone.get(), &Rclone::finished, this, [rclone, this](int exit)
        {
            if (exit == 0)
            {
                m_layout->removeWidget(m_loading1);
                delete m_loading1;
                auto json = boost::json::parse(rclone->getData().find("json")->second);
                m_layout->addWidget(new QLabel(tr("Total : ")), row, 0, 1, 2);
                uint64_t size = json.at("total").as_int64();
                m_layout->addWidget(new QLabel(
                        (Iridium::Utility::numberToString(size) + " octets (" + Iridium::Utility::sizeToString(size) +
                         ")").c_str()), row, 1, 1, 1);
                row++;

                m_layout->addWidget(new QLabel(tr("Utilisé : ")), row, 0, 1, 2);
                size = json.at("used").as_int64();
                m_layout->addWidget(new QLabel(
                        (Iridium::Utility::numberToString(size) + " octets (" + Iridium::Utility::sizeToString(size) +
                         ")").c_str()), row, 1, 1, 1);
                row++;

                m_layout->addWidget(new QLabel(tr("Libre : ")), row, 0, 1, 2);
                size = json.at("free").as_int64();
                m_layout->addWidget(new QLabel(
                        (Iridium::Utility::numberToString(size) + " octets (" + Iridium::Utility::sizeToString(size) +
                         ")").c_str()), row, 1, 1, 1);

                // label wrap
                for (auto const &lab: findChildren<QLabel *>())
                    lab->setWordWrap(true);
            }
        });
        rclone->about(*m_file->getRemoteInfo());
        return;
    }

    initSize();

    m_layout->addWidget(new QLabel(tr("Date de modification: ")), row, 0, 1, 1);
    m_layout->addWidget(m_mod_time, row, 1, 1, 1);
    row++;

    if (m_file->isDir())
    {
        m_size->hide();
        m_layout->addWidget(new QLabel(tr("Nombre d'objets: ")), row, 0, 1, 1);
        m_layout->addWidget(m_objs, row, 1, 1, 1);
        m_loading1 = new ProgressBar(ProgressBar::Circular, this);
        m_loading1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_loading1->setRange(0, 0);
        m_layout->addWidget(m_loading1, 5, 1, 1, 1, Qt::AlignLeft);

        m_loading2 = new ProgressBar(ProgressBar::Circular, this);
        m_loading2->setRange(0, 0);
        m_loading2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_layout->addWidget(m_loading2, 7, 1, 1, 1, Qt::AlignLeft);
    }


    for (auto const &lab: findChildren<QLabel *>())
        lab->setTextInteractionFlags(Qt::TextSelectableByMouse);

}

void ItemInfoDialog::initLabel()
{
    m_icon = new QLabel(this);
    m_icon->setAlignment(Qt::AlignHCenter);
    m_icon->setMinimumHeight(200);

    m_path = new QLabel(m_file->getPath(), this);
    m_path->setWordWrap(true);
    m_path->setAlignment(Qt::AlignLeft);

    m_name = new QLabel(m_file->getName(), this);
    m_name->setWordWrap(true);
    m_name->setAlignment(Qt::AlignLeft);

    m_mod_time = new QLabel(m_file->getModTimeString(), this);
    m_mod_time->setWordWrap(true);
    m_mod_time->setAlignment(Qt::AlignLeft);

    if (m_file->isDir())
    {
        m_objs = new QLabel("", this);
        m_objs->hide();
        m_objs->setWordWrap(true);
        m_objs->setAlignment(Qt::AlignLeft);
    }
}


void ItemInfoDialog::initSize()
{
    m_layout->addWidget(new QLabel(tr("Taille: ")), row, 0, 1, 1);
    m_size = new QLabel("", this);

    if (m_file->getRemoteInfo()->isLocal() and m_file->isDir())
    {
        auto *size = new uint64_t(0);
        auto *objs = new uint64_t(0);
        m_thread = boost::make_shared<boost::thread>(
                [this, objs, size]
                {
//			 qDirIerator recursive
                    QDirIterator it(m_file->getPath(),
                                    QDir::Files|QDir::System|QDir::Hidden,
                                    QDirIterator::Subdirectories);
                    for (; it.hasNext(); it.next())
                    {
                        boost::this_thread::interruption_point();
                        QFileInfo info(it.fileInfo());
                        if (!info.isDir())
                        {
                            (*size) += info.size();
                            (*objs)++;
                        }
                    }
                    emit m_threadFinished();
                });

        connect(this, &ItemInfoDialog::m_threadFinished, this, [this, objs, size]()
        {
            m_layout->removeWidget(m_loading1);
            m_loading1->deleteLater();
            m_layout->removeWidget(m_loading2);
            m_loading2->deleteLater();
            m_size->show();
            m_objs->show();

            m_objs->setText(QString::fromStdString(Utility::numberToString((uint64_t) *objs)));
            m_size->setText(QString::fromStdString(Utility::numberToString(*size)) + " octets" + " ("
                            + QString::fromStdString(Utility::sizeToString(*size)) + ")");
            m_file->setSize(*size);
            m_file->setObjs(*objs);
            m_objs->setFont({});
            m_size->setFont({});
            delete size;
            delete objs;
        });
    }
    if (!m_file->getRemoteInfo()->isLocal() and m_file->getSize() == 0)
    {

        auto rclone = RcloneManager::get();
        connect(rclone.get(), &Rclone::sizeFinished, this,
                [this](const uint32_t &objs, const uint64_t &size, const std::string &strSize)
                {
                    m_layout->removeWidget(m_loading1);
                    m_loading1->deleteLater();
                    m_layout->removeWidget(m_loading2);
                    m_loading2->deleteLater();
                    m_size->show();
                    m_objs->show();

                    m_size->setText(
                            QString::fromStdString(Utility::numberToString(size)) + " octets" + " (" + strSize.c_str() +
                            ")");
                    m_objs->setText(QString::fromStdString(Utility::numberToString((uint64_t) objs)));
                    m_file->setSize(size);
                    m_file->setObjs(objs);
                    m_objs->setFont({});
                    m_size->setFont({});
                });
        rclone->size(m_file->getPath().toStdString());
    } else
    {
        m_size->setText(QString::fromStdString(Utility::numberToString(m_file->getSize())) + " octets" + " (" +
                        m_file->getSizeString() + ")");
        if (m_file->isDir())
        {
            m_objs->setText(QString::fromStdString(Utility::numberToString((uint64_t) m_file->getObjs())));
        }
    }
    m_size->setWordWrap(true);
    m_size->setAlignment(Qt::AlignLeft);
    m_layout->addWidget(m_size, row, 1, 1, 1);
    row++;
}

void ItemInfoDialog::initType()
{
    m_layout->addWidget(new QLabel(tr("Type: ")), row, 0, 1, 1);
    m_type = new QLabel(m_file->getFileType(), this);
    m_type->setWordWrap(true);
    m_type->setAlignment(Qt::AlignLeft);
    m_layout->addWidget(m_type, row, 1, 1, 1);
    row++;

}

ItemInfoDialog::~ItemInfoDialog()
{
    if (not m_thread)
        return;
    if (m_thread->joinable())
    {
        m_thread->interrupt();
        m_thread->join();
    }
}