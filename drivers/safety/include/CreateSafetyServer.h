#pragma once

#include <vector>
#include <string>

#include "Safety.h"

SafetySP CreateSafetyServer(const std::string &publish,const std::vector < std::string > &subscribes, float rate, float delayOff);

