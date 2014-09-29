/*
 * TestVRep.cpp
 *
 *  Created on: Sep 27, 2014
 *      Author: kcastlet
 */
#include <iostream>
#include <cmath>
#include <map>
#include <string>

extern "C" {
#include "extApi.h"
	/*	#include "extApiCustom.h" if you wanna use custom remote API functions! */
}

using namespace std;

int main(int argc,char **argv) {
	int portNb = 19997;//This is the default port V-REP connects to on startup without need for starting a temporary connection in a lua script during simulation
	int clientID=simxStart((simxChar*)"127.0.0.1",portNb,true,true,2000,5);
	float theta=0;
	if (clientID!=-1)
	{
		simxAddStatusbarMessage(clientID,"EXTERNAL CONNECTION MADE!",simx_opmode_oneshot_wait);

		//One time setup code here

		map<string,int> joints;
        string ids[]={"Cogburn_11","Cogburn_12","Cogburn_13",
        		"Cogburn_21","Cogburn_22","Cogburn_23",
        		"Cogburn_31","Cogburn_32","Cogburn_33",
        		"Cogburn_41","Cogburn_42","Cogburn_43"};
        for (int i=0;i<12;i++){
          int temp;
		  simxGetObjectHandle(clientID,ids[i].c_str(),&temp,simx_opmode_oneshot_wait);
		  joints[ids[i]]=temp;
        }
		simxStartSimulation(clientID,simx_opmode_oneshot_wait);
		float angle=0.0;
		while (simxGetConnectionId(clientID)!=-1 && theta<=2*3.1415)
		{
			//simulation code here.

			extApi_sleepMs(5);
			angle+=10.0*M_PI/180.0;
			for(int i=0;i<12;i++) {
			  simxSetJointPosition(clientID,joints[ids[i]],angle,simx_opmode_oneshot_wait);
			}

		}
		simxStopSimulation(clientID,simx_opmode_oneshot_wait);

		//one time cleanup before closing connection.

		simxFinish(clientID);
	}
	else
	{
		//cout << "No Connection!" << endl; // prints
	}
  return 0;
}


