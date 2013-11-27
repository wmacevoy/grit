#pragma once

class LidarMessage
{
 public:
  double t; // seconds
  float waist,neckud,necklr; // degrees
  enum { SIZE = 1081 };
  float data[SIZE]; // inches (1e6 for no strike)
};

