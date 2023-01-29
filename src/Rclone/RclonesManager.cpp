//
// Created by rahman on 29/01/23.
//

#include "RclonesManager.hpp"

using namespace std;

RclonesManager::RclonesManager(int nbMaxProcess) : nbMaxProcess(nbMaxProcess)
{}

RclonesManager::RclonesManager() : nbMaxProcess(100)
{}

std::shared_ptr<Rclone> RclonesManager::get()
{
	rcloneVector.push_back(make_shared<Rclone>())
}
