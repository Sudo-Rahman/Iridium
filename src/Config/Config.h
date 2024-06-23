#pragma once

#include <string>
#include "Version.h"

static const char* IRIDIUM_VERSION = Iridium_VERSION;
static const char* AUTHOR = "Rahman YILMAZ";
static const auto CPP_VERSION = "C++" + std::to_string(__cplusplus).substr(2, 2);