#include <Servo.h>

#define MAXPOS 10
#define TIMESCALE 125

class Leg {
  boolean invert;
  Servo hip,coxa,knee;
  public:
  byte h,c,k;
  void attach(int hipPin,int coxaPin,int kneePin,boolean newInvert) {
    knee.attach(kneePin);
    coxa.attach(coxaPin);
    hip.attach(hipPin);
    invert=newInvert;
  }
  void park() {
    h=90;
    c=90;
    k=90;
  }
  void update() {
    if (invert) {
      hip.write(h);
      coxa.write(180-c);
      knee.write(180-k);
    }
    else {
      hip.write(180-h);
      coxa.write(c);
      knee.write(k);
    }
  }
  void paw(long time) {
    int s=(time/100) % 120;
    c=90+s-60;
    k=90+s-60;
  }
};

class LegSequence {
  byte hAngle[MAXPOS];
  byte cAngle[MAXPOS];
  byte kAngle[MAXPOS];
  int time[MAXPOS];
  protected:
  byte pos;
  long total,offset;
  boolean reverse,interp;
  public:
  virtual void init(long newOffset,boolean newReverse,boolean newInterp) {
   total=0;
   pos=0;
   offset=newOffset;
   reverse=newReverse;
   interp=newInterp;
  }
  void add(int h,int c,int k,long t) {
    if (pos==MAXPOS) return;
    hAngle[pos]=h;
    cAngle[pos]=c;
    kAngle[pos]=k;
    time[pos]=t;
    total+=t;
    pos++;
  }
  void pose(Leg &l) {
    long s=(millis()+offset) % total;
    if (reverse) s=total-s;
    for (int i=0;i<pos;i++) {
      if (s<time[i]) {
        float t=(float)s/(float)time[i];
        if (!interp) t=0.0;
        int nh=hAngle[0];
        int nc=cAngle[0];
        int nk=kAngle[0];
        if (i+1<pos) {
         nh=hAngle[i+1];
         nc=cAngle[i+1];
         nk=kAngle[i+1]; 
        }
        l.h=hAngle[i]+t*(nh-hAngle[i]);
        l.c=cAngle[i]+t*(nc-cAngle[i]);
        l.k=kAngle[i]+t*(nk-kAngle[i]);
        break;
      } else
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

void compute3D(float x,float y,float z,float l,float &knee,float &femur,float &hip,boolean invert) {
  float h=sqrt(x*x+z*z);
  hip=180-acos(x/h)*180.0/M_PI;
  compute2D(h,y,l,knee,femur);
  if (invert) {
    hip=180.0-hip;
  }  
}

class Dog:public LegSequence {
  public:
  Dog(float xstart,float xstop,float dy,float l,int total) {
    pos=0;
    boolean invert=false;
    float femur,knee,hip;
    int fast=total/4; // fast phase time
    int slow=total-fast; // slow phase time
    int dt=slow/(MAXPOS-2);
    for (int i=0;i<MAXPOS-2;i++) {
      float dx=(xstop-xstart)/(float)(MAXPOS-2);
      float x=dx*(float)i+xstart;
      compute2D(x,l,l,knee,femur);

      add(115,(int)femur,(int)knee,dt);
    }
    compute2D(xstop,l-dy,l,knee,femur);
    add(115,(int)femur,(int)knee,fast/2);
    compute2D(xstart,l-dy,l,knee,femur);
    add(115,(int)femur,(int)knee,fast/2);    
  }
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    offset=newOffset;
    reverse=newReverse;
    interp=true;
  }
};


class Crab:public LegSequence {
  public:
  Crab(float xstart,float xstop,float y,float z,float dy,float l,int total) {
    pos=0;
    boolean invert=false;
    float femur,knee,hip;
    int fast=total/4; // fast phase time
    int slow=total-fast; // slow phase time
    int dt=slow/(MAXPOS-2);
    for (int i=0;i<MAXPOS-2;i++) {
      float dx=(xstop-xstart)/(float)(MAXPOS-2);
      float x=dx*(float)i+xstart;
      compute3D(x,y,z,l,knee,femur,hip,invert);

      add((int)hip,(int)femur,(int)knee,dt);
    }
    compute3D(xstop,y-dy,z,l,knee,femur,hip,invert);
    add((int)hip,(int)femur,(int)knee,fast/2);
    compute3D(xstart,y-dy,z,l,knee,femur,hip,invert);
    add((int)hip,(int)femur,(int)knee,fast/2);    
  }
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    offset=newOffset;
    reverse=newReverse;
    interp=true;
  }
};

class Frozen:public LegSequence {
  public:
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    LegSequence::init(newOffset,newReverse,newInterp);
    add(135,90,90,1500);
  }
};

class FrozenPark:public LegSequence {
  public:
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    LegSequence::init(newOffset,newReverse,newInterp);
    add(135,35,90,1500);
  }
};

class FrozenTall:public LegSequence {
  public:
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    LegSequence::init(newOffset,newReverse,newInterp);
//    add(170,170,170,1500);  // narrow in walk direction
//    add(170,10,135,1500);   // base on ground legs up narrow
    add(90,170,170,31000);
    add(90,150,150,1000);
  }
};


class Quad {
  LegSequence *lsrf,*lslf,*lsrb,*lslb;
  public:
  Leg rf,lf,rb,lb;
  void setSequence(
    LegSequence *nrf=NULL,
    LegSequence *nlf=NULL,
    LegSequence *nrb=NULL,
    LegSequence *nlb=NULL) {
      lsrf=nrf;
      lslf=nlf;
      lsrb=nrb;
      lslb=nlb;
  }
  void attach() {
    lf.attach(4,3,2,false);
    rf.attach(7,6,5,true);
    lb.attach(10,9,8,true);
    rb.attach(13,12,11,false);
    setSequence();
  }
  void pose() {
      if (lsrf!=NULL && 
        lslf!=NULL && 
        lsrb!=NULL && 
        lslb!=NULL) {
          lsrf->pose(rf);
          lslf->pose(lf);
          lsrb->pose(rb);
          lslb->pose(lb);
      }
  }
  void park() {
    rf.park();
    lf.park();
    rb.park();
    lb.park();
  }
  void update() {
    rf.update();
    lf.update();
    rb.update();
    lb.update();
  }
};

Quad myQuad;
Crab rb1(0.,1.,1.,.75,0.5,1.0,4*TIMESCALE),lb1(0.,1.,1.,.75,0.5,1.0,4*TIMESCALE);
Crab rf1(1.,0.,1.,.75,0.5,1.0,4*TIMESCALE),lf1(1.,0.,1.,.75,0.5,1.0,4*TIMESCALE);
Dog rb2(1.5,0.5,.5,1.0,4*TIMESCALE),lb2(0.5,1.5,.5,1.0,4*TIMESCALE);
Dog rf2(1.5,0.5,.5,1.0,4*TIMESCALE),lf2(0.5,1.5,.5,1.0,4*TIMESCALE);
Frozen rf3,lf3,rb3,lb3;
FrozenPark rf4,lf4,rb4,lb4;
FrozenTall rf5,lf5,rb5,lb5;
long start;

void setup() { 
  Serial.begin(9600);
  myQuad.attach();
  myQuad.park();
  rf1.init(0);
  lb1.init(TIMESCALE);
  rb1.init(2*TIMESCALE);
  lf1.init(3*TIMESCALE);
  rf2.init(0);
  lb2.init(TIMESCALE);
  rb2.init(2*TIMESCALE);
  lf2.init(3*TIMESCALE);
  rf3.init(1);
  lb3.init(1);
  rb3.init(1);
  lf3.init(1);
  rf4.init(1);
  lb4.init(1);
  rb4.init(1);
  lf4.init(1);
  rf5.init(0);
  lb5.init(8000);
  rb5.init(16000);
  lf5.init(24000);

  myQuad.setSequence(&rf3,&lf3,&rb3,&lb3);
}

void loop() { 
  char command[3];
  myQuad.update();
  if (Serial.available()) {
    Serial.readBytesUntil(0x0d,command,3);
    if (command[0]=='f') {
      myQuad.setSequence(&rf3,&lf3,&rb3,&lb3);
    } else if (command[0]=='d') {
        myQuad.setSequence(&rf2,&lf2,&rb2,&lb2);
    } else if (command[0]=='c') {
        myQuad.setSequence(&rf1,&lf1,&rb1,&lb1);
    } else if (command[0]=='p') {
        myQuad.setSequence(&rf4,&lf4,&rb4,&lb4);
    } else if (command[0]=='h') {
        myQuad.setSequence(&rf5,&lf5,&rb5,&lb5);
    }
  }
  long now=millis();
  myQuad.pose();
} 
