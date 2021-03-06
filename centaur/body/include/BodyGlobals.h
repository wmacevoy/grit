#pragma once

#include <string>
#include <memory>
#include <atomic>


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
#include "BodyMover.h"
#include "SensorsMessage.h"
#include "CreateSafetyClient.h"
#include "Vec3d.h"
#include "Mat3d.h"
#include "Arc3d.h"

extern bool verbose;
extern double simTime;
extern double lastSimTime;
extern double simSpeed;
extern double realTime;
extern double lastRealTime;

extern std::map < std::string , double > strs;
extern std::map < std::string , double > nums;
extern std::map < std::string , Vec3d >  vecs;
extern std::map < std::string , Mat3d >  mats;
extern std::map < std::string , Arc3d >    arcs;

extern std::shared_ptr < Configure > cfg;

extern SPServoController servoController;
extern std::map < int , SPServo > servos;
extern SPBody body;
extern SPBodyMover mover;
extern SafetySP safety;

extern SensorsMessage sensors;

extern SPServo servo(std::string name);
