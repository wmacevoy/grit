#pragma once

class SensorMessage
{
 public:
  double t;
  // x,y,z
  float a[3];

  // wx, wy, wz
  float g[3];

  // leg1,..,leg4
  float p[4];
};
