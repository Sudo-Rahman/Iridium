#pragma once

#include <vector>
#include <QVariant>

#include "RcloneFile.hpp"

class SearchRow
{
public:
	explicit SearchRow(const RcloneFilePtr &file);

	RcloneFilePtr file() const;

	auto data(int column) const -> QVariant;

	auto compare(int column, Qt::SortOrder order, const SearchRow &other) const -> bool;

private:
	RcloneFilePtr _file;
};
