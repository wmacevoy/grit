#include <csignal>
#include <iostream>
#include <memory>
#include <math.h>
#include <string.h>

#include "CreateFakeServoController.h"
#include "CreateZMQServoController.h"
#include "CreateDynamixelServoController.h"
#include "Configure.h"
#include "now.h"

using namespace std;

Configure cfg;
bool verbose;

volatile bool running = true;

void SigIntHandler(int arg) {
  running = false;
}

int main(int argc, char **argv)
{
  cfg.path("../../setup");
  cfg.args("servos.",argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("servos.verbose",false);
  if (verbose) cfg.show();

  string use_controller="zmq";
  string me = "tcp://*:5501";
  string server = "tcp://localhost:5500";
  int deviceIndex = 0;
  int baudNum = 34;
  double minAngle = -45.0;
  double maxAngle =  45.0;
  double speed = 0.1;
  int rate = 20;
  int servoId=1;
  bool verbose = false;
  
  signal(SIGINT, SigIntHandler);

  for (int argi=1; argi<argc; ++argi) {
    if (strcmp(argv[argi],"--zmq")==0) {
      use_controller = "zmq";
      continue;
    }
    if (strcmp(argv[argi],"--dynamixel")==0) {
      use_controller = "dynamixel";
      continue;
    }
    if (strcmp(argv[argi],"--fake")==0) {
      use_controller = "fake";
      continue;
    }
    if (strcmp(argv[argi],"--servo")==0) {
      ++argi;
      servoId = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--deviceIndex")==0) {
      ++argi;
      deviceIndex = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--baudNum")==0) {
      ++argi;
      baudNum = atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--me")==0) {
      ++argi;
      me=argv[argi];
      continue;
    }
    if (strcmp(argv[argi],"--server")==0) {
      ++argi;
      server=argv[argi];
      continue;
    }
    if (strcmp(argv[argi],"--rate")==0) {
      ++argi;
      rate=atoi(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--min")==0) {
      ++argi;
      minAngle=atof(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--max")==0) {
      ++argi;
      maxAngle=atof(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--speed")==0) {
      ++argi;
      speed=atof(argv[argi]);
      continue;
    }
    if (strcmp(argv[argi],"--verbose")==0) {
      verbose = true;
      continue;
    }
    if (strcmp(argv[argi],"--help")==0) {
      cout << "usage: " << argv[0] << " options" << endl;
      cout << "\t --help (print help)"  << endl;
      cout << "\t --verbose"  << endl;
      cout << "\t --fake (use fake controller)" << endl;
      cout << "\t --zmq (use zmq controller)" << endl;
      cout << "\t --dynamixel (use direct dynamixel controller)" << endl;
      cout << "\t --servo [id] (use this servo id)" << endl;
      cout << "\t --me [address] (zmq publish address)" << endl;
      cout << "\t --server [address] (zmq subscribe address)" << endl;
      cout << "\t --deviceIndex [num] (dynamixel device index)" << endl;
      cout << "\t --baudNum [num] (dynamixel baud number)" << endl;
      cout << "\t --min [degrees] (min servo angle degrees)" << endl;
      cout << "\t --max [degrees] (min servo angle degrees)" << endl;
      cout << "\t --speed [cycles/sec] (servo speed)" << endl;
      cout << "\t --rate [number] (updates per second)" << endl;
      return 0;
    }
    cout << "unkown arg '" << argv[argi] << "' ignored."  << endl;
  }

  shared_ptr<ServoController> controller;
  if (use_controller == "fake") {
    cout << "fake controller" << endl;
    controller = shared_ptr<ServoController>(CreateFakeServoController());
  }
  if (use_controller == "zmq") {
    cout << "zmq controller at " << me << " talking to " << server << endl;
    controller = shared_ptr<ServoController>(CreateZMQServoController(me,server));    
  }
  if (use_controller == "dynamixel") {
    cout << "dynamixel controller device index " << deviceIndex << " baud number " << baudNum << endl;
    controller = shared_ptr<ServoController>(CreateDynamixelServoController(cfg,deviceIndex,baudNum));    
  }
  shared_ptr<Servo> servo(controller->servo(servoId));

  controller->start();
  double t0 = now();

  while (running) {
    usleep(int((1.0/rate)*1000000));
    double t=now()-t0;
    double goalAngle = 
      (minAngle+maxAngle)/2.0+(maxAngle-minAngle)/2.0*(sin(speed*2*M_PI*t));
    servo->angle(goalAngle);
    double presentAngle = servo->angle();
    if (verbose) {
      cout << "t=" << t << " at " << presentAngle << " goal " << goalAngle << endl;
    }
  }

  return 0;
}
