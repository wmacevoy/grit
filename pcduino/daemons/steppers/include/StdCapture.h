#pragma once

// http://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string

#include <string>

class StdCapture
{
 public:
  StdCapture();
  ~StdCapture();
  void BeginCapture();
  bool EndCapture();
  std::string GetCapture() const;
 private:
  enum PIPES { READ, WRITE };
  bool m_capturing;
  bool m_init;
  int m_oldStdOut;
  int m_oldStdErr;
  int m_pipe[2];
  std::string m_captured;
};
