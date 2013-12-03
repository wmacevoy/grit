#pragma once
#include <string>
#include <memory>

#include "HandTapeEditor.h"
#include "ArmGeometry.h"
#include "Mat3d.h"

class ArmTapeEditor : public HandTapeEditor
{
 public:
  std::shared_ptr < ArmGeometry > geometry;
  std::string shoulderio,shoulderud,bicep,elbow,forearm;

  // side is LEFTARM or RIGHTARM
  ArmTapeEditor(const std::string &side_); 
  void pose(const Mat3d &frame);
  void line(const Mat3d &frame0, const Mat3d &frame1, float speed, bool first, bool last);

  using TapeEditor::parse;

  virtual bool parse(const std::string &cmd, std::istream &in);
};

