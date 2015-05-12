#include "Foot.h"

void testFoot()
{
  printf("In Foot Test\n");
  Foot f(2,3,4,5,6,7);
	
  while(true)  
    {
      usleep(100000);
    }
}

int main(int argc, char** argv)
{
  system("sudo modprobe adc");
  printf("In Foot\n");
  testFoot();
  return 0;
}
