//
// Created by sr-71 on 07/05/2023.
//

#ifndef IRIDIUM_GLOBAL_HPP
#define IRIDIUM_GLOBAL_HPP

#include <RcloneFile.hpp>

namespace Iridium
{
    enum Load
    {
        Dynamic, Static
    };

    struct Global
    {
        static std::vector<RcloneFilePtr> copy_files;
        static std::vector<RemoteInfoPtr> remotes;
        static uint8_t max_process;
        static std::string path_rclone;
        static Load load_type;
        static uint8_t max_depth;

        static void clear_and_swap_copy_files(const std::vector<RcloneFilePtr> &files)
        {
            copy_files.swap(const_cast<std::vector<RcloneFilePtr> &>(files));
        }
    };
};


#endif //IRIDIUM_GLOBAL_HPP
