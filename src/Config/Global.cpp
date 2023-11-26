//
// Created by sr-71 on 07/05/2023.
//

#include "Global.hpp"
#include <thread>

using namespace std;
using namespace Iridium;


vector<RcloneFilePtr> Global::copy_files{};
vector<RcloneFilePtr> Global::sync_dirs{};
vector<RemoteInfoPtr> Global::remotes{};
map<RemoteInfoPtr , any> Global::remote_model{};

uint8_t Global::max_process = thread::hardware_concurrency();
string Global::path_rclone;
Load Global::load_type = Load::Dynamic;
uint8_t Global::max_depth = 2;
uint8_t Global::reload_time = 10;