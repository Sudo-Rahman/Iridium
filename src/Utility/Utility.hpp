//
// Created by rahman on 12/02/23.
//

#ifndef IRIDIUM_UTILITY_HPP
#define IRIDIUM_UTILITY_HPP

#include <iostream>
#include <string>
#include <vector>

typedef long double double64_t;

namespace Iridium
{
	/**
	 * @brief The Utility class
	 */
	class Utility
	{

	public:
		[[nodiscard]] static std::string numberToString(uint64_t num, int space = 3);

		[[nodiscard]] static std::string numberToString(double64_t num, int space = 3);

		[[nodiscard]] static std::string sizeToString(double64_t num);
	};

}
#endif //IRIDIUM_UTILITY_HPP
