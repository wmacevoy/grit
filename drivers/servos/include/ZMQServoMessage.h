#pragma once

class ZMQServoMessage
{
public:
  enum { SET_ANGLE, GET_ANGLE,
	 SET_SPEED,
	 SET_TORQUE
  };

  int messageId;
  int servoId;
  float value;
};
