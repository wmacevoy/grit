#pragma once

struct SensorsMecanumMessage
{
  uint16_t speed;
  uint8_t  directions[4];
  uint8_t  enabled;
};
