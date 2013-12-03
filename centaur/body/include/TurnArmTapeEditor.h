#pragma once

#include <string>

#include "ArmTapeEditor.h"
#include "Arc3d.h"

class TurnArmTapeEditor : public ArmTapeEditor
{
 public:
  Arc3d arc; /* path to follow */
  float dr; /* inches away on start/release/restart */
  float speed; /* inches/sec turn rate */

  TurnArmTapeEditor(const std::string &side_, 
		    const Arc3d &arc_, 
		    float dr_=2.0, 
		    float speed_=4.0);

  void start();
  void grasp();
  void turn();
  void release();
  void restart();
  void cycle();
  void cycles(int n);

  using TapeEditor::parse;

  virtual bool parse(const std::string &cmd, std::istream &in);
};
