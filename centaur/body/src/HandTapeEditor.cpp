#include "HandTapeEditor.h"

#include <assert.h>

HandTapeEditor::HandTapeEditor(const std::string &side_)
{
  side=side_;
  assert((side  == "LEFTARM") || (side == "RIGHTARM"));

  thumb=side + "_THUMB";
  trigger=side + "_TRIGGER";
  middle=side + "_MIDDLE";
  ring=side + "_RING";

  names.push_back(thumb);
  names.push_back(trigger);
  names.push_back(middle);
  names.push_back(ring);
}

void HandTapeEditor::close()
{
  if (!tape()) return;
  s += 3;
  paths(trigger)[s]=360;
  paths(middle)[s]=360;
  paths(ring)[s]=360;
  s += 3;
  paths(thumb)[s]=360;
}

void HandTapeEditor::open()
{
  if (!tape()) return;
  s += 3;
  paths(thumb)[s]=0;
  s += 3;
  paths(trigger)[s]=0;
  paths(middle)[s]=0;
  paths(ring)[s]=0;
}

bool HandTapeEditor::parse(const std::string &cmd, std::istream &in)
{
  if (TapeEditor::parse(cmd,in)) { return true; }
  if (cmd == "open") { open() ; return true; }
  if (cmd == "close") { close(); return true; }
  return false;
}
