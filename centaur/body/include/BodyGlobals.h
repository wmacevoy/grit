#pragma once

#include <string>
#include <memory>


#define MOVE_RATE 50.0
#define ANGLE360 360.0
#define ANGLE180 (ANGLE360/2.0)
#define ANGLE90 (ANGLE180/2.0)
#define ANGLE45 (ANGLE90/2.0)
#define HIPAXISX 5.707
#define HIPAXISY 5.707

#define LEG1 0
#define LEG2 1
#define LEG3 2
#define LEG4 3

#include "Configure.h"
#include "ServoController.h"
#include "Body.h"

typedef std::shared_ptr < ServoController > SPServoController;
typedef std::shared_ptr < Servo > SPServo;
typedef std::shared_ptr < Body > SPBody;

extern bool verbose;

extern double simTime;
extern double simSpeed;
extern double realTime;
extern Configure cfg;

extern SPServoController servoController;
extern SPBody body;

extern SPServo servo(std::string name);