#pragma once

class ZMQServoMessage
{
public:
  enum { SET_ANGLE, GET_ANGLE };

  int messageId;
  int servoId;
  float value;
};
