//
// Created by sr-71 on 07/05/2023.
//

#include "Variable.hpp"
#include <thread>

using namespace std;
using namespace Iridium;


vector<RcloneFilePtr> Variable::copy_files = {};
atomic_int_fast8_t Variable::nb_rclone_locked = 0;
uint8_t Variable::nb_max_process = thread::hardware_concurrency();