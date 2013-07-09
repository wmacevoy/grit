#include <iostream>
#include <unistd.h>
#include "DynamixelDriver.hpp"

using namespace std;

int main()
{
  int id = 1;
  
  for (;;) {
    int goal=rand()%4096;
    sleep(1);
    DXL2USB.sendWord(id,DXL_GOAL_POSITION_WORD,goal);
    sleep(1);
    int pos=DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD);
    cout << "goal=" << goal << " pos=" << pos << endl;
  }
}
