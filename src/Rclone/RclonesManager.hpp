//
// Created by rahman on 29/01/23.
//

#ifndef IRIDIUM_RCLONESMANAGER_HPP
#define IRIDIUM_RCLONESMANAGER_HPP

#include "Rclone.hpp"
#include <iostream>
#include <memory>

class RclonesManager
{
	std::atomic<int> nb_rclones{};
	int  nbMaxProcess{};
	std::mutex mutex{};
	std::condition_variable conditionVariable{};
	std::vector<std::shared_ptr<Rclone>> rcloneVector{};

public:
	explicit RclonesManager(int nbMaxProcess);
	explicit RclonesManager();

	std::shared_ptr<Rclone> get();

};


#endif //IRIDIUM_RCLONESMANAGER_HPP
