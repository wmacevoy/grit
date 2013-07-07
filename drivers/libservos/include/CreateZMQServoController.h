#pragma once

#include <string>
#include "ServoController.h"

ServoController* CreateZMQServoController(const std::string &me,
					  const std::string &server);

