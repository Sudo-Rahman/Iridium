//
// Created by rahman on 12/02/23.
//

#include <sstream>
#include <cmath>
#include "Utility.hpp"

#include "Settings.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace Iridium;

/**
 * @brief Utility::numberToString, coverti un nombre en string avec des espaces
 * @param num
 * @param space
 * @return string
 */
string Utility::numberToString(int64_t num, int space)
{
	string tmpSize;

	stringstream ss;
	ss << num;
	string str = ss.str();
	int i = space;
	for (std::string::iterator it = str.end(); it != str.begin(); --it)
	{
		if (it != str.end())
		{
			tmpSize = *it + tmpSize;
			if (i-- == 1)
			{
				tmpSize = " " + tmpSize;
				i = space;
			}
		}
	}
	tmpSize = str.front() + tmpSize;
	return tmpSize;
}

std::string Utility::numberToString(uint64_t num, int space) { return numberToString((int64_t)num, space); }

/**
 * @brief Utility::numberToString, converti un nombre en string avec des espaces
 * @param num
 * @param space
 * @return string
 */
string Utility::numberToString(double64_t num, int space)
{
	string tmpSize;

	stringstream ss;
	ss << num;
	string str = ss.str();
	auto str1 = str.substr(0, str.find('.'));
	auto str2 = str.substr(str.find('.'), str.size());
	tmpSize += numberToString(stoll(str2), space);
	tmpSize += numberToString(stoll(str1), space);
	return tmpSize;
}

/**
 * @brief Utility::sizeToString, converti une taille en string avec des unités
 * @param size
 * @return string
 */
string Utility::sizeToString(double64_t size)
{
	if (size / pow(1024, 4.0) > 1)
	{
		stringstream ss;
		ss << int(size / pow(1024.0, 4.0) * 100) / 100.0 << " TiB";
		return ss.str();
	}
	if (size / pow(1024, 3.0) > 1)
	{
		stringstream ss;
		ss << int(size / pow(1024.0, 3.0) * 100) / 100.0 << " GiB";
		return ss.str();
	}

	if (size / pow(1024, 2.0) > 1)
	{
		stringstream ss;
		ss << int(size / pow(1024.0, 2.0) * 100) / 100.0 << " MiB";
		return ss.str();
	}
	if (size / 1024.0 > 1)
	{
		stringstream ss;
		ss << int(size / pow(1024.0, 1.0) * 100) / 100.0 << " KiB";
		return ss.str();
	}
	stringstream ss;
	ss << size << " B";
	return ss.str();
}

/**
 * @brief Utility::pushBack, ajoute des éléments à un vecteur
 * @param dst
 * @param src
 * @param ...
 */
void Utility::pushBack(vector<std::string> &dst, const std::vector<std::vector<std::string>> &src)
{
	for (auto &i: src)
	{
		for (auto &j: i)
			dst.push_back(j);
	}
}

bool Utility::isPreviewable(const RcloneFile &file)
{
	if (Settings::getSystem().os != Settings::MacOs)
		return false;
	if (file.isDir())
		return false;
	if(not file.getRemoteInfo()->isLocal())
	{
		// max 50 Mo
		if (file.getSize() > 1024 * 1024 * 50 or file.getSize() == 0)
			return false;
	}
	for (auto &i: file.mimeTypes())
	{
		if (i.name().contains("image") or i.name().contains("video") or i.name().contains("audio") or i.name().
		    contains("text") or i.name().contains("pdf"))
			return true;
	}
	return false;
}

bool Utility::isPreviewable(const QList<RcloneFilePtr> &files)
{
	for (auto &i: files)
	{
		if (not isPreviewable(*i))
			return false;
	}
	return true;
}
