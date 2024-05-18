//
// Created by rahman on 12/02/23.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include "RcloneFile.hpp"

typedef long double double64_t;

using std::string;

namespace Iridium
{
	/**
	 * @brief The Utility class
	 */
	class Utility
	{
	public:
		[[nodiscard]] static std::string numberToStringInt(int64_t num, int space = 3);

		[[nodiscard]] static std::string numberToStringFloating(double64_t num, int space = 3);

		template<typename T>
		[[nodiscard]] static string numberToString(T num, int space = 3) requires std::is_arithmetic_v<T>
		{
			if (std::is_floating_point_v<T>) { return Utility::numberToStringFloating(num, space); }
			return Utility::numberToStringInt(num, space);
		}

		[[nodiscard]] static std::string sizeToString(double64_t num);

		static void pushBack(std::vector<std::string> &dst, const std::vector<std::vector<std::string>> &src);

	};
}
