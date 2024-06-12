//
// Created by sr-71 on 12/06/2024.
//

#include "SearchRow.hpp"

#include "Utility/Utility.hpp"

SearchRow::SearchRow(const RcloneFilePtr &file) : _file(file) {}

RcloneFilePtr SearchRow::file() const { return _file; }

auto SearchRow::data(int column) const -> QVariant
{
	switch (column)
	{
		case 0:
			return _file->getName();
		case 1:
			return _file->getRemoteInfo()->name().c_str();
		case 2:
			return _file->parent_dir().c_str();
		case 3:
			return Iridium::Utility::sizeToString(_file->getSize()).c_str();
		case 4:
			return _file->getModTimeString();
		case 5:
			return _file->getFileType();
		default:
			return QVariant();
	}
}

auto SearchRow::compare(int column, Qt::SortOrder order, const SearchRow &other) const -> bool
{
	switch (column)
	{
		case 0:
			if (order == Qt::AscendingOrder)
				return _file->getName() < other._file->getName();
			return _file->getName() > other._file->getName();
		case 1:
			if (order == Qt::AscendingOrder)
				return _file->getRemoteInfo()->name() < other._file->getRemoteInfo()->name();
			return _file->getRemoteInfo()->name() > other._file->getRemoteInfo()->name();
		case 2:
			if (order == Qt::AscendingOrder)
				return _file->parent()->path() < other._file->parent()->path();
			return _file->parent()->path() > other._file->parent()->path();
		case 3:
			if (order == Qt::AscendingOrder)
				return _file->getSize() < other._file->getSize();
			return _file->getSize() > other._file->getSize();
		case 4:
			if (order == Qt::AscendingOrder)
				return _file->mod_time().time_since_epoch().count() < other._file->mod_time().time_since_epoch().count();
			return _file->mod_time().time_since_epoch().count() > other._file->mod_time().time_since_epoch().count();
		case 5:
			if (order == Qt::AscendingOrder)
				return _file->getFileType() < other._file->getFileType();
			return _file->getFileType() > other._file->getFileType();
		default:
			return false;
	}
}
