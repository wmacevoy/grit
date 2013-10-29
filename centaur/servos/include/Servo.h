#pragma once

#include <memory>
#include <string>

#define USE_SERVO_LINEAR 0

class Servo
{
private:
  int m_id;
  std::string m_name;
public:
  virtual void curve(double t[2], float c0[3],float c1[3])=0;
  virtual float angle() const=0; // degrees
  virtual void  angle(float value)=0;
  virtual void  speed(float value)=0; // degrees/sec
  virtual void  torque(float value)=0; // fraction of maximum, zero is disabled.
  virtual void  rate(float value)=0; // hertz
  virtual ~Servo();
  virtual void id(int id_);
  virtual void name(const std::string &name_);
  virtual const std::string& name() const;
  virtual int id() const;
  virtual void report(std::ostream &out) const;
  virtual void temp_report(std::ostream &out) const;
  virtual uint8_t temp() const=0;
};

typedef std::shared_ptr < Servo > SPServo;
