#pragma once

class DynamixelServo
{
  bool wheelMode;
  float presentAngle;
  void init(newId, std::string newName);
  void wheel(int speed);
  void setTorque(int value);
  void report();
  void angle(float newAngle);
  float angle() const;
}
