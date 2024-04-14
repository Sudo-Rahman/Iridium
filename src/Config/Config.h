#pragma once

#include <string>
#include <vector>
#include "Version.h"

static std::string const IRIDIUM_VERSION = Iridium_VERSION;
static std::string const AUTHOR = "Rahman YILMAZ";

static std::vector<std::string> const OTHER_RESSOURCES_AUHTOR = {
        "https://icones8.fr/icons",
        "https://fr.freepik.com/vecteurs-libre/pack-modeles-logo-degrade_12980862.htm"
};

static std::string CPP_VERSION = "C++" + std::to_string(__cplusplus).substr(2, 2);