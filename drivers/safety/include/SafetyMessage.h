#pragma once

#include <stdint.h>

struct SafetyMessage
{
  enum { SET_SAFE, GET_SAFE, GET_WARN, GET_STALE };
  int8_t messageId;
  int8_t value;
};

