#include <Servo.h> 

#define MAXPOS 10

class Leg {
  boolean invert;
  Servo hip,coxa,knee;
  public:
  int h,c,k;
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
  int denominator,numerator;
  byte hAngle[MAXPOS];
  byte cAngle[MAXPOS];
  byte kAngle[MAXPOS];
  long time[MAXPOS];
  protected:
  int pos;
  long total,offset;
  boolean reverse,interp;
  public:
  void setSpeed(int newNumerator,int newDenominator) {
    numerator=newNumerator;
    denominator=newDenominator;
  }
  virtual void init(long newOffset,boolean newReverse,boolean newInterp) {
   pos=0; 
   total=0;
   offset=newOffset;
   reverse=newReverse;
   interp=newInterp;
   denominator=1;
   numerator=1;
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
//    long s=(((millis()+offset)*numerator)/denominator) % total;
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

class FrontCrab:public LegSequence {
  public:
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    LegSequence::init(newOffset,newReverse,newInterp);
    add(140,90,90,1500);
    add(110,90,90,1000);
    add(90,90,90,1000);
    add(90,0,0,250);
    add(140,0,0,250);
  }
};
 
class BackCrab:public LegSequence {
  public:
  void init(long newOffset,boolean newReverse=false,boolean newInterp=false){
    LegSequence::init(newOffset,newReverse,newInterp);
    add(90,90,90,1500);
    add(110,90,90,1000);
    add(140,90,90,1000);
    add(140,0,0,250);
    add(90,0,0,250);
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
//    =180.0-femur;
  }  
}

class Dog:public LegSequence {
  public:
  Dog(float xstart,float xstop,float y,float z,float dy,float l,int total) {
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
    add(90,90,90,1500);
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
  void change(char *change) {
    char s=change[0];
    char t=change[1]-'0';
    char n=change[2]-'0';
    n=t*10+n;
    if (s=='u') {
      rf.c+=n; lf.c+=n; rb.c+=n; lb.c+=n;
    }
    if (s=='d') {
      rf.c-=n; lf.c-=n; rb.c-=n; lb.c-=n;
    }
    if (s=='r') {
      rf.h-=n; lf.h+=n; rb.h+=n; lb.h-=n;
    }
    if (s=='l') {
      rf.h+=n; lf.h-=n; rb.h-=n; lb.h+=n;
    }
    if (s=='o') {
      rf.k+=n; lf.k+=n; rb.k+=n; lb.k+=n;
    }
    if (s=='i') {
      rf.k-=n; lf.k-=n; rb.k-=n; lb.k-=n;
    }    
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
  void setSpeed(int num,int den) {
    if (lsrf!=NULL && 
        lslf!=NULL && 
        lsrb!=NULL && 
        lslb!=NULL) {
      lsrf->setSpeed(num,den);
      lslf->setSpeed(num,den);
      lsrb->setSpeed(num,den);
      lslb->setSpeed(num,den);
    }
  }
};

Quad myQuad;
BackCrab lbc,rbc;
FrontCrab lfc,rfc;
Dog rbd(-.5,1.0,1.5,.7,.5,1.0,500),lbd(-.5,1.0,1.5,.7,.5,1.0,500);
Dog rfd(1.0,-.5,1.5,.7,.5,1.0,500),lfd(1.0,-.5,1.5,.7,.5,1.0,500);
//Frozen rfd,lfd;
//Frozen rbd,lbd;
long start;

void setup() { 
  Serial.begin(9600);
  myQuad.attach();
  myQuad.park();
//  rfc.init(0);
//  rbc.init(1000);
//  lfc.init(2000);
//  lbc.init(3000);
  rfd.init(0);
  lbd.init(125);
  lfd.init(250);
  rbd.init(325);
//  myQuad.setSequence(&rfc,&lfc,&rbc,&lbc);
  myQuad.setSequence(&rfd,&lfd,&rbd,&lbd);
//  myQuad.setSpeed(2,1);
}

void loop() { 
  char command[3];
  myQuad.update();
  if (Serial.available()) {
    Serial.readBytesUntil(0x0d,command,3);
    myQuad.change(command);
  }
  long now=millis();
  myQuad.pose();
} 
