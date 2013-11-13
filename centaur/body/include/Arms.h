#pragma once

#include <memory>
#include "Servo.h"

class Arm
{
 public:
  std::shared_ptr < Servo > inOut;
  std::shared_ptr < Servo > upDown;
  std::shared_ptr < Servo > bicep;
  std::shared_ptr < Servo > elbow; 
  std::shared_ptr < Servo > forearm; 
  std::shared_ptr < Servo > trigger; 
  std::shared_ptr < Servo > middle; 
  std::shared_ptr < Servo > ring; 
  std::shared_ptr < Servo > thumb; 
  virtual void init()=0;
  void goLimp();
  void report(std::ostream &out) const;
  void temp_report(std::ostream &out) const;
};

class LeftArm:public Arm {
  public:
  void init();
};

class RightArm:public Arm {
  public:
  void init();
};
