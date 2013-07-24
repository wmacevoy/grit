#include "config.h"
#include "ScaledServo.h"

#if SERVO_CURVE == 1
void ScaledServo::curve(double t0, float c0[3],float c1[3])
{
  float s0[3];
  float s1[3];
  s0[0]=slope*c0[0]+offset;
  s0[1]=slope*c0[1];
  s0[2]=slope*c0[2];

  s1[0]=slope*c1[0]+offset;
  s1[1]=slope*c1[1];
  s1[2]=slope*c1[2];
  
  unscaledServo->curve(t0,s0,s1);
}
#endif

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
