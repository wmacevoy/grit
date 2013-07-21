#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

#include "now.h"
#include "CreateZMQServoController.h"
#include "CreateDynamixelServoController.h"
#include "CreateFakeServoController.h"

using namespace std;

double epoch;

bool move(Servo *servo, float goal, float speed)
{
  usleep(int(0.75*1000000));

  double t0=now()-epoch;
  float at0=servo->angle();

  cout << "at " << at0 << " at t0=" << t0 << endl;

  // motions less than 1/2 second are too fast to test
  if (fabs((at0 - goal)/speed) < 0.500) {
    return true;
  }

  servo->speed(speed);
  servo->angle(goal);

  double dt=fabs((at0-goal)/speed);

  usleep(int(dt/2.0*1000000));
  
  double t1=now()-epoch;
  float at1=servo->angle();
  float speed1=(at1-at0)/(t1-t0);

  cout << "at " << at1 << " after dt=" << t1-t0 << endl;
  cout << "computed speed at dt/2 of " << speed1 << endl;
  cout << "desired speed of " << speed << endl;

  // if speed at half-time is 50% off, then wrong
  if (fabs((fabs(speed1)-fabs(speed))/speed) > 0.50) {
    return false;
  }

  usleep(int(dt/2.0*1000000));
  float at2=servo->angle();

  cout << "computed location at dt of " << at2 << endl;
  cout << "desired location of " << goal << endl;

  // if time to goal is more than 0.25 seconds off, then wrong
  if (fabs((at2-goal)/speed) > 0.25) {
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  epoch=now();

  ServoController *controller=0;
  vector<int> servo_ids;
  vector<Servo*> servos;

  float minAngle = -60.0;
  float maxAngle =  60.0;
  float minSpeed =  15.0;
  float maxSpeed =  90.0;

  float torque = 1.0;
  int trials = 10;

  for (int argi=1; argi<argc; ++argi) {

    if (strcmp(argv[argi],"--dynamixel")==0) {
      int deviceIndex = atoi(argv[++argi]);
      int baudNum = atoi(argv[++argi]);
      controller = CreateDynamixelServoController(deviceIndex,baudNum);
      continue;
    }

    if (strcmp(argv[argi],"--fake")==0) {
      controller = CreateFakeServoController();
      continue;
    }

    if (strcmp(argv[argi],"--zmq")==0) {
      char *me = argv[++argi];
      char *server = argv[++argi];
      controller = CreateZMQServoController(me,server);
      continue;
    }

    if (strcmp(argv[argi],"--servos")==0) {
      string arg=argv[++argi];
      while (arg.length() > 0) {
	size_t comma = arg.find(',');
	string part = arg.substr(0,(comma != string::npos) ? comma : arg.length());
	arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
	servo_ids.push_back(atoi(part.c_str()));
      }
      continue;
    }

    if (strcmp(argv[argi],"--minAngle")==0) {
      minAngle=atof(argv[++argi]);
      continue;
    }

    if (strcmp(argv[argi],"--maxAngle")==0) {
      maxAngle=atof(argv[++argi]);
      continue;
    }

    if (strcmp(argv[argi],"--minSpeed")==0) {
      minSpeed=atof(argv[++argi]);
      continue;
    }

    if (strcmp(argv[argi],"--maxSpeed")==0) {
      maxSpeed=atof(argv[++argi]);
      continue;
    }

    if (strcmp(argv[argi],"--torque")==0) {
      torque=atof(argv[++argi]);
      continue;
    }

    if (strcmp(argv[argi],"--trials")==0) {
      trials=atoi(argv[++argi]);
      continue;
    }
    
    if (strcmp(argv[argi],"--help")==0) {
      cout << "usage " << argv[0] << " args..." << endl;
      cout << "\t --help" << endl;
      cout << "\t --servos ids,..." << endl;
      cout << "\t --dynamixel deviceId baudNum" << endl;
      cout << "\t --fake" << endl;
      cout << "\t --zmq me/publish listen/subscribe" << endl;
      cout << "\t --minAngle deg" << endl;
      cout << "\t --maxAngle deg" << endl;
      cout << "\t --minSpeed deg/sec" << endl;
      cout << "\t --maxSpeed deg/sec" << endl;
      cout << "\t --torque fraction" << endl;
      return 0;
    }

    cout << "unknown arg '" << argv[argi] << "', try --help." << endl;
    return 0;
  }

  if (controller == 0) {
    controller = CreateFakeServoController();
  }

  if (servo_ids.size() == 0) {
    servo_ids.push_back(1);
  }

  for (size_t i=0; i != servo_ids.size(); ++i) {
    servos.push_back(controller->servo(servo_ids[i]));
  }

  controller->start();

  for (size_t i=0; i != servos.size(); ++i) {
    servos[i]->torque(torque);
    servos[i]->angle(0);
    servos[i]->speed(maxSpeed);
  }

  sleep(1);


  for (size_t i=0; i != servos.size(); ++i) {
    for (int trial=0; trial<trials; ++trial) {
      float goal = minAngle+(maxAngle-minAngle)*(rand()/double(RAND_MAX));
      float speed = minSpeed+(maxSpeed-minSpeed)*(rand()/double(RAND_MAX));
      cout << "test " << trial << " on servo " << servo_ids[i] << " goal=" << goal << " speed=" << speed << endl;
      if (!move(servos[i],goal,speed)) {
	cout << "failed." << endl;
	return 1;
      }
    }
  }

  delete controller;

  cout << "all tests passed." << endl;

  return 0;
}
