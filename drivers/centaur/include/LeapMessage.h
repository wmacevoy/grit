#pragma once

struct LeapHandMessage
{
  float at[3]; // inches
  float point[3]; // unit vector
  float down[3];  // unit vector
};

struct LeapMessage 
{
  struct LeapHandMessage left;
  struct LeapHandMessage right;
};
