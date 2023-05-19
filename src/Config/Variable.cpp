//
// Created by sr-71 on 07/05/2023.
//

#include "Variable.hpp"
#include <thread>

using namespace std;
using namespace Iridium;


vector<RcloneFilePtr> Variable::copy_files = {};
vector<RemoteInfoPtr> Variable::remotes{};
uint8_t Variable::max_process = thread::hardware_concurrency();