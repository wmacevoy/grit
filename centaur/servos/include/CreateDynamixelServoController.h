#pragma once

#include "ServoController.h"
#include "Configure.h"

ServoController* CreateDynamixelServoController(Configure &cfg, int deviceIndex,int baudNum);

