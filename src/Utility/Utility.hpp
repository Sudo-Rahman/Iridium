//
// Created by rahman on 12/02/23.
//

#ifndef IRIDIUM_UTILITY_HPP
#define IRIDIUM_UTILITY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include "RcloneFile.hpp"

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

        static void pushBack(std::vector<std::string> &dst, const std::vector<std::vector<std::string>> &src);

        static bool isPreviewable(const RcloneFile &file);

        static bool isPreviewable(const QList<RcloneFilePtr> &files);

        template<class OBJ>
        [[nodiscard]] static std::string addressObject (const OBJ &obj )
        {
            std::stringstream ss;
            ss << &obj;
            return ss.str();
        }

        template<class OBJ>
        [[nodiscard]] static std::string addressObject (const OBJ *obj )
        {
            return addressObject(*obj);
        }

    };

}
#endif //IRIDIUM_UTILITY_HPP
