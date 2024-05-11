//
// Created by Rahman on 11/02/2023.
//

#include "ItemInfoDialog.hpp"
#include <Utility/Utility.hpp>
#include <QDirIterator>
#include <QPropertyAnimation>
#include <IridiumApp.hpp>

using namespace Iridium;

ItemInfoDialog::ItemInfoDialog(TreeFileItem * item, QWidget * parent) : QDialog(parent), _item(item)
{
	// delete on close
	setAttribute(Qt::WA_DeleteOnClose);
	_file = item->getFile();
	setMaximumSize(500, 600);
	setMinimumWidth(300);
	setWindowTitle(tr("Information"));
	setModal(false);
	_layout = new QGridLayout(this);
	_layout->setColumnStretch(1, 1);
	_layout->setVerticalSpacing(20);
	_layout->setAlignment(Qt::AlignTop);


	initLabel();

	_icon->setPixmap(item->icon().pixmap(128, 128));
	_layout->addWidget(_icon, _row, 0, 2, 2);
	_row += 2;

	initType();

	_layout->addWidget(new QLabel(tr("Chemin Absolu: ")), _row, 0, 1, 1);
	_layout->addWidget(_path, _row, 1, 1, 1);
	_row++;

	_layout->addWidget(new QLabel(tr("Nom: ")), _row, 0, 1, 1);
	_layout->addWidget(_name, _row, 1, 1, 1);
	_row++;

	if (_item->index().model()->index(0, 0) == _item->index())
	{
		_name->setText(_file->getRemoteInfo()->name().c_str());
		_mod_time->hide();
		_loading1 = new ProgressBar(ProgressBar::Circular, this);
		_loading1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		_loading1->setFixedSize(100, 100);
		_loading1->setRange(0, 0);
		_layout->addWidget(_loading1, _row, 0, 1, 3, Qt::AlignCenter);

		auto parser = ir::parser::about_parser::create(
			new ir::parser::about_parser([&](const ire::about& about)
			{
				IridiumApp::runOnMainThread([about = std::move(about), this]
				{
					_layout->removeWidget(_loading1);
					delete _loading1;
					_layout->addWidget(new QLabel(tr("Total : ")), _row, 0, 1, 2);
					_layout->addWidget(new QLabel(
						                   (Utility::numberToString(about.total_bytes) + " octets (" +
						                    Utility::sizeToString(about.total_bytes) +
						                    ")").c_str()), _row, 1, 1, 1);
					_row++;

					_layout->addWidget(new QLabel(tr("Utilisé : ")), _row, 0, 1, 2);
					_layout->addWidget(new QLabel(
						                   (Utility::numberToString(about.used_bytes) + " octets (" +
						                    Utility::sizeToString(about.used_bytes) +
						                    ")").c_str()), _row, 1, 1, 1);
					_row++;

					_layout->addWidget(new QLabel(tr("Libre : ")), _row, 0, 1, 2);
					_layout->addWidget(new QLabel(
						                   (Utility::numberToString(about.free_bytes) + " octets (" +
						                    Utility::sizeToString(about.free_bytes) +
						                    ")").c_str()), _row, 1, 1, 1);

					// label wrap
					for (auto const& lab: findChildren<QLabel *>())
						lab->setWordWrap(true);
				});
			}));


		_process.about(*_file->getRemoteInfo())
				.on_finish_parser(parser).execute();
		return;
	}

	initSize();

	_layout->addWidget(new QLabel(tr("Date de modification: ")), _row, 0, 1, 1);
	_layout->addWidget(_mod_time, _row, 1, 1, 1);
	_row++;

	if (_file->isDir())
	{
		_layout->addWidget(new QLabel(tr("Nombre d'objets: ")), _row, 0, 1, 1);
		_layout->addWidget(_objs, _row, 1, 1, 1);
		if (_file->getSize() <= 0)
		{
			_size->hide();
			_loading1 = new ProgressBar(ProgressBar::Circular, this);
			_loading1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			_loading1->setFixedSize(48, 48);
			_loading1->setRange(0, 0);
			_layout->addWidget(_loading1, 5, 1, 1, 1, Qt::AlignLeft);

			_loading2 = new ProgressBar(ProgressBar::Circular, this);
			_loading2->setRange(0, 0);
			_loading2->setFixedSize(48, 48);
			_loading2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			_layout->addWidget(_loading2, 7, 1, 1, 1, Qt::AlignLeft);
		}
	}


	for (auto const& lab: findChildren<QLabel *>())
		lab->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

void ItemInfoDialog::initLabel()
{
	_icon = new QLabel(this);
	_icon->setAlignment(Qt::AlignHCenter);
	_icon->setMinimumHeight(200);

	_path = new QLabel(_file->absolute_path().c_str(), this);
	_path->setWordWrap(true);
	_path->setAlignment(Qt::AlignLeft);

	_name = new QLabel(_file->getName(), this);
	_name->setWordWrap(true);
	_name->setAlignment(Qt::AlignLeft);

	_mod_time = new QLabel(_file->getModTimeString(), this);
	_mod_time->setWordWrap(true);
	_mod_time->setAlignment(Qt::AlignLeft);

	if (_file->isDir())
	{
		_objs = new QLabel("", this);
		if (_file->getSize() <= 0)
			_objs->hide();
		_objs->setWordWrap(true);
		_objs->setAlignment(Qt::AlignLeft);
	}
}


void ItemInfoDialog::initSize()
{
	_layout->addWidget(new QLabel(tr("Taille: ")), _row, 0, 1, 1);
	_size = new QLabel("", this);

	if (_file->getRemoteInfo()->isLocal() and _file->isDir())
	{
		auto * size = new uint64_t(0);
		auto * objs = new uint64_t(0);
		_thread = boost::make_shared<boost::thread>(
			[this, objs, size]
			{
				//			 qDirIerator recursive
				QDirIterator it(_file->getPath(),
				                QDir::Files | QDir::System | QDir::Hidden,
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
				emit threadFinished();
			});

		connect(this, &ItemInfoDialog::threadFinished, this, [this, objs, size]()
		{
			_size->show();
			_objs->show();

			_objs->setText(QString::fromStdString(Utility::numberToString(*objs)));
			_size->setText(QString::fromStdString(Utility::numberToString(*size)) + " octets" + " ("
			               + QString::fromStdString(Utility::sizeToString(*size)) + ")");
			_file->setSize(*size);
			_objs->setFont({});
			_size->setFont({});
			delete size;
			delete objs;
		});
	}
	if (!_file->getRemoteInfo()->isLocal() and _file->getSize() <= 0 and _file->isDir())
	{
		auto parser = ir::parser::size_parser::create(new ir::parser::size_parser([this](const ire::size& size)
		{
			IridiumApp::runOnMainThread([this,size = std::move(size)]
			{
				_layout->removeWidget(_loading1);
				_loading1->deleteLater();
				_layout->removeWidget(_loading2);
				_loading2->deleteLater();
				_size->show();
				_objs->show();

				_size->setText(
					QString::fromStdString(Utility::numberToString(size.total_size)) + " octets" + " (" +
					std::to_string(size.total_size).c_str() +
					")");
				_objs->setText(QString::fromStdString(Utility::numberToString(size.total_objects)));
				_objs->setFont({});
				_size->setFont({});
			});
		}));
		_process.size(*_file).on_finish_parser(parser).execute();
	}
	else
	{
		_size->setText(QString::fromStdString(Utility::numberToString(_file->getSize())) + " octets" + " (" +
		               Utility::sizeToString(_file->size()).c_str() + ")");
		if (_file->isDir())
		{
			_objs->setText(QString::fromStdString(Utility::numberToString((uint64_t)_file->children().size())));
		}
	}
	_size->setWordWrap(true);
	_size->setAlignment(Qt::AlignLeft);
	_layout->addWidget(_size, _row, 1, 1, 1);
	_row++;
}

void ItemInfoDialog::initType()
{
	_layout->addWidget(new QLabel(tr("Type: ")), _row, 0, 1, 1);
	_type = new QLabel(_file->getFileType(), this);
	_type->setWordWrap(true);
	_type->setAlignment(Qt::AlignLeft);
	_layout->addWidget(_type, _row, 1, 1, 1);
	_row++;
}

ItemInfoDialog::~ItemInfoDialog()
{
	if (_process.is_running())
		_process.stop();
	if (not _thread)
		return;
	if (_thread->joinable())
	{
		_thread->interrupt();
		_thread->join();
	}
}
