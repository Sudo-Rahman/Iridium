//
// Created by sr-71 on 07/05/2023.
//

#ifndef IRIDIUM_VARIABLE_HPP
#define IRIDIUM_VARIABLE_HPP

#include <RcloneFile.hpp>

namespace Iridium
{

    struct Variable
    {
        static std::vector<RcloneFilePtr> copy_files;

        static std::atomic_int_fast8_t nb_rclone_locked;
        static uint8_t nb_max_process;

        static void clear_and_swap_copy_files(const std::vector<RcloneFilePtr> &files)
        {copy_files.swap(const_cast<std::vector<RcloneFilePtr> &>(files));}
    };
};


#endif //IRIDIUM_VARIABLE_HPP
