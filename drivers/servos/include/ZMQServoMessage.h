#pragma once

class ZMQServoMessage
{
public:
  enum { SET_ANGLE, GET_ANGLE,
	 SET_SPEED,
	 SET_TORQUE,
	 SET_CURVE,
	 DO_NOTHING
  };

  int messageId;
  int servoId;
  float value;
};

// extended message for curves...
class ZMQServoCurveMessage
{
 public:
  int messageId;
  int servoId;
  double t[2];
  float c0[3];
  float c1[3];
};
