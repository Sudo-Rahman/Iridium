//
// Created by rahman on 12/02/23.
//


#include <sstream>
#include <cmath>
#include "Utility.hpp"

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
string Utility::numberToString(uint64_t num, int space)
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
	tmpSize += numberToString((uint64_t) stoull(str2), space);
	tmpSize += numberToString((uint64_t) stoull(str1), space);
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
