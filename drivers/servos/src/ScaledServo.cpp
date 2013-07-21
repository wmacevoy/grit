#include "ScaledServo.h"

void ScaledServo::init(std::shared_ptr < Servo > unscaledServo_, float slope_, float offset_)
{
  unscaledServo=unscaledServo_;
  slope=slope_;
  offset=offset_;
}

ScaledServo::ScaledServo(std::shared_ptr < Servo > unscaledServo_, float slope_, float offset_)
  : unscaledServo(unscaledServo_), slope(slope_), offset(offset_)
{

}


