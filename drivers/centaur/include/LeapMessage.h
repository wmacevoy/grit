#pragma once

struct LeapHandMessage
{
  float at[3];
  float point[3];
  float down[3];
};

struct LeapMessage 
{
  struct LeapHandMessage left;
  struct LeapHandMessage right;
};
