#include <iostream>
#include <termio.h>
#include <stdio.h>
#include "DynamixelDriver.hpp"

using namespace std;

class Leg {
	Servo knee,femur,hip;
	string name;
	int kneePos,femurPos,hipPos;
public:
	void init(int kneeid,int femurid,int hipid,string newName) {
	  name=newName;
	  knee.init(kneeid,name+"k");
	  femur.init(femurid,name+"f");
	  hip.init(hipid,name+"h");
	}
	void setPos(int newKnee=2048,int newFemur=2048,int newHip=2048) {
	  kneePos=newKnee;
	  femurPos=newFemur;
	  hipPos=newHip;
	  knee.joint(kneePos);
	  femur.joint(femurPos);
	  hip.joint(hipPos);
	}
	void setTorque(int torque) {
		knee.setTorque(torque);
		femur.setTorque(torque);
		hip.setTorque(torque);
	}
	void report() {
		knee.report();
		femur.report();
		hip.report();
	}
};

class Quad {
	Leg l1,l2,l3,l4;
public:
	void init(int torque) {
	  l1.init(11,12,13,"l1");
	  l2.init(21,22,23,"l2");
	  l3.init(31,32,33,"l3");
	  l4.init(41,42,43,"l4");
	  l1.setTorque(torque);
	  l2.setTorque(torque);
	  l3.setTorque(torque);
	  l4.setTorque(torque);
	}
	void report() {
		l1.report();
		l2.report();
		l3.report();
		l4.report();
	}
    void setPosAll(int knee,int femur,int hip){
    	l1.setPos(knee,femur,hip);
    	l2.setPos(knee,femur,hip);
    	l3.setPos(knee,femur,hip);
    	l4.setPos(knee,femur,hip);
    }
};

int main()
{
    Quad legs;
    legs.init(512);

	int k=2048;
	int f=2048;
	int h=2048;
	legs.setPosAll(k,f,h);
	while(1)
	{
		cout <<"Press Enter key to continue!(press q and Enter to quit)"<<endl;
		char key=getchar();
		if( key == 'q')
			break;
        if(key=='k')k-=100;
        if(key=='j')k+=100;
        if (k<0)k=0;
        if (k>4095)k=4095;
        if(key=='f')f-=100;
        if(key=='d')f+=100;
        if (f<0)f=0;
        if (f>4095)f=4095;
        if(key=='h')h-=100;
        if(key=='g')h+=100;
        if (h<0)h=0;
        if (h>4095)h=4095;
        if (key=='1') {
            k=4000;
            f=95;
            h=2048;
        }
        if (key=='2'){
          k=2648;
          f=2048;
          h=2048;
        }
        if (key=='3'){
          k=95;
          f=4000;
          h=2048;
        }
        if (key=='r'){
          legs.report();
        }
        legs.setPosAll(k,f,h);
        key=' ';
		cout << "h:" <<h<<" f: "<<f<<" k:"<< k << endl;
	}
	return 0;
}
