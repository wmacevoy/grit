#pragma once

#include <string>
#include "ServoController.h"

ServoController* CreateZMQServoListener(const std::string &server);
