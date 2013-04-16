#include <iostream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
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

const int MAXPOS=10;

class LegSequence {
  int hAngle[MAXPOS];
  int fAngle[MAXPOS];
  int kAngle[MAXPOS];
  long time[MAXPOS];
  protected:
  int pos;
  long total,offset;
  bool reverse,interp;
  public:
  LegSequence() {
    init(0,false,false);
  }
  virtual void init(long newOffset,bool newReverse,bool newInterp) {
   total=0;
   pos=0;
   offset=newOffset;
   reverse=newReverse;
   interp=newInterp;
  }
  void add(int k,int f,int h,long t) {
    if (pos>=MAXPOS) return;
    hAngle[pos]=h;
    fAngle[pos]=f;
    kAngle[pos]=k;
    time[pos]=t;
    total+=t;
    pos++;
  }
  void pose(Leg &l) {
 //   long millis=std::time(0);
    clock_t t;
    t = clock();
    long long millis=(((long long)t)*1000)/CLOCKS_PER_SEC;
    long s=(millis+offset) % total;
   // l.report();
   // cout <<"s" <<s << endl;
    if (reverse) s=total-s;
    for (int i=0;i<pos;i++) {
      if (s<time[i]) {
        float t=(float)s/(float)time[i];
        if (!interp) t=0.0;
        int nf=fAngle[0];
        int nh=hAngle[0];
        int nk=kAngle[0];
        if (i+1<pos) {
         nf=fAngle[i+1];
         nh=hAngle[i+1];
         nk=kAngle[i+1];
        }
        int f=fAngle[i]+t*(nf-fAngle[i]);
        int h=hAngle[i]+t*(nh-hAngle[i]);
        int k=kAngle[i]+t*(nk-kAngle[i]);
        l.setPos(k,f,h);
        return;
      }
      s-=time[i];
    }
  }
};

void compute2D(float x,float y,float l,float &knee,float &femur) {
  float h=sqrt(x*x+y*y);
  float theta1=atan(x/y)*180.0/M_PI;
  knee=2.0*asin((h/2.0)/l)*180.0/M_PI; // assumes femur=tibia=l
  float theta2=180.0-90.0-knee/2;
  femur=180.0-theta1-theta2;
}

void compute3D(float x,float y,float z,float l,float &knee,float &femur,float &hip,bool invert) {
  float h=sqrt(x*x+z*z);
  hip=180-acos(x/h)*180.0/M_PI;
  compute2D(h,y,l,knee,femur);
  if (invert) {
    hip=180.0-hip;
  }
}

class Crab:public LegSequence {
public:
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,true);
		add(2048,2048,2048,1500);
		add(2448,2048,1548,500);
		add(2448,1023,1548,1500);
		add(2048,1023,2048,500);
	}
};

class Center:public LegSequence {
public:
	void init(int offset,bool reverse,bool interp) {
		LegSequence::init(offset,reverse,interp);
		add(2048,2048,2048,1000);
	}
};


class Quad {
	Leg l1,l2,l3,l4;
	LegSequence *l1s,*l2s,*l3s,*l4s;
	Center f1,f2,f3,f4;
public:
	Quad() {
		f1.init(0,false,false);
		f2.init(0,false,false);
		f3.init(0,false,false);
		f4.init(0,false,false);
		setSequences(&f1,&f2,&f3,&f4);
	}
	void setSequences(LegSequence *newl1s,LegSequence *newl2s,LegSequence *newl3s,LegSequence *newl4s){
		l1s=newl1s;
		l2s=newl2s;
		l3s=newl3s;
		l4s=newl4s;
	}
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
    void programmed(){
    	l1s->pose(l1);
    	l2s->pose(l2);
    	l3s->pose(l3);
    	l4s->pose(l4);
;    }
};

int main()
{
    Quad legs;
    legs.init(512);
    Crab   l1s,l2s,l3s,l4s;
    l1s.init(0,false,true);
    l2s.init(2000,false,true);
    l3s.init(1000,false,true);
    l4s.init(3000,false,true);
    legs.setSequences(&l1s,&l2s,&l3s,&l4s);

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
        if (key=='p'){
          while (true) {
            legs.programmed();
          }
        }
        legs.setPosAll(k,f,h);
        key=' ';
		cout << "h:" <<h<<" f: "<<f<<" k:"<< k << endl;
	}
	return 0;
}
