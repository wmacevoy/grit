#include <iostream>
#include "Configure.h"

using namespace std;

Configure cfg;

char *args[] = { "--aname", "avalue" , 0 };

int main(int argc, char *argv[])
{
  cfg.path("../../setup");
  cfg.args("test.",args);
  if (argc == 1) cfg.load("config.csv");
  cfg.show();
  cfg.servos();
  
  cout << "leg1 knee offset=" << cfg.servo("LEG1_KNEE","offset") << endl;

  return 0;
}
