#pragma once

class SensorsMessage
{
 public:
  int32_t t;
  // x,y,z
  int32_t a[3];

  // x,y,z
  int32_t c[3];

  // wx, wy, wz
  int32_t g[3];

  // leg1,..,leg4
  int32_t p[4];

  // status -- safe(output), warn(output)
  int16_t s[2];
};
