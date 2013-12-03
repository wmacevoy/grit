#pragma once
#include <string>
#include "Tape.h"
#include "TapeEditor.h"

class HandTapeEditor : public TapeEditor
{
 public:
  std::string side;
  std::string thumb,trigger,middle,ring;

  

  // side is LEFTARM or RIGHTARM
  HandTapeEditor(const std::string &side_); 
  
  void close();
  void open();

  using TapeEditor::parse;

  virtual bool parse(const std::string &cmd, std::istream &in);
};

