//
// Created by Rahman on 14/04/2023.
//

#ifndef IRIDIUM_CONFIG_H
#define IRIDIUM_CONFIG_H

#include <string>
#include <vector>
#include "Version.h"

enum BuildType
{
    Debug,
    Release,
};

static BuildType type(int type)
{
    switch (type)
    {
        case 0:
            return BuildType::Debug;
        case 1:
            return BuildType::Release;
        default:
            return BuildType::Debug;
    }
}

static std::string const IRIDIUM_VERSION = Iridium_VERSION;
static std::string const AUTHOR = "Rahman YILMAZ";

static std::vector<std::string> const OTHER_RESSOURCES_AUHTOR = {
        "https://icones8.fr/icons",
        "https://fr.freepik.com/vecteurs-libre/pack-modeles-logo-degrade_12980862.htm"
};

static std::string CPP_VERSION = "C++" + std::to_string(__cplusplus).substr(2, 2);

static BuildType const BUILD_TYPE = type(std::stoi(Build_TYPE));


#endif //IRIDIUM_CONFIG_H
