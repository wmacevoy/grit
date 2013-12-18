%module zmqservos
%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr(Servo)
%shared_ptr(ZMQServo)
%shared_ptr(ServoController)
%{
#define SWIG_FILE_WITH_INIT
#include "Servo.h"
#include "ZMQServo.h"
#include "ZMQServoMessage.h"
#include "ServoController.h"
#include "CreateFakeServoController.h"
#include "CreateZMQServoController.h"
#include "CreateZMQServoListener.h"
%}
%include "Servo.h"
%include "ZMQServo.h"
%include "ZMQServoMessage.h"
%include "ServoController.h"
%include "CreateFakeServoController.h"
%include "CreateZMQServoController.h"
%include "CreateZMQServoListener.h"

