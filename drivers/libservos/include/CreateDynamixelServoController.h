#pragma once

#include "ServoController.h"

ServoController* CreateDynamixelServoController(int DeviceNumber, int BaudRate);

