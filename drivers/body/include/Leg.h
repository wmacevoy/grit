#pragma once

class Leg : public LegGeometry
{
  std::shared_ptr < Servo > knee;
  std::shared_ptr < Servo > femur;
  std::shared_ptr < Servo > hip;

  void init(std::shared_ptr < ServoController > &controller, int kneeid,int femurid,int hipid,string newName);

  void setEnd(const Point &tip)  
}
