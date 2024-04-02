#pragma once

#include <any>
#include <RcloneFile.hpp>
#include <iridium/process.hpp>

namespace Iridium
{
    enum Load
    {
        Dynamic, Static
    };

    struct Global
    {
        static std::vector<RcloneFilePtr> copy_files;
        static std::vector<RcloneFilePtr> sync_dirs;
        static std::vector<RemoteInfoPtr> remotes;
        static uint8_t max_process;
        static std::string path_rclone;
        static Load load_type;
        static uint8_t max_depth;
        static uint8_t reload_time;
        static ir::process_pool process_pool;
        static std::map<RemoteInfoPtr , std::any> remote_model;

        static void clear_and_swap_copy_files(const std::vector<RcloneFilePtr> &files)
        {
            copy_files.swap(const_cast<std::vector<RcloneFilePtr> &>(files));
        }

        static void add_process(ir::process_uptr &process);
    };
};