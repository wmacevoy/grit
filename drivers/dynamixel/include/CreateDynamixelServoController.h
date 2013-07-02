#pragma once

#include "ServoController.h"

ServoController* CreateDynamixelServoController(int deviceIndex,int baudNum);
ServoController* CreateDynamixelServoController();

