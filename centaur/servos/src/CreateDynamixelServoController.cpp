#include <sstream>
#include <strstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <map>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <zmq.h>
#include <string.h>

#include "ServoController.h"
#include "Configure.h"

#define USE_BROADCAST 1

#if USE_BROADCAST == 1
#include <dynamixel.h>
#endif

#include "now.h"

// http://support.robotis.com/en -- MX-106t (legs)  MX-28t (neck & fingers)
//
// http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm
#include "DXLIO.h"

#define USE_TORQUE_ENABLED 1
#define SPEED_FACTOR 1.1

using namespace std;

struct DynamixelServo : Servo
{
  DXLIO &io;
  int id;
  int presentPosition;
  int minAngle;
  int maxAngle;
  int presentSpeed;
  int presentTorque;
  int goalPosition;
  int goalSpeed;
  int goalTorque;
  float minSpeed;
  float maxSpeed;
  float minTorque;
  float maxTorque;
  bool verbose;
  uint8_t presentTemp;
  bool enabled;
  bool curveMode;
  double t[2];
  float c0[3],c1[3];
  float rxTempRate;
  float rxPositionRate;

  DynamixelServo(Configure &cfg, DXLIO &io_, int id_) 
    : io(io_),id(id_), presentPosition(2048), goalPosition(2048) 
  {
    enabled=true;
    minSpeed = atof(cfg.servo(id,"minspeed").c_str());
    maxSpeed = atof(cfg.servo(id,"maxspeed").c_str());
    minTorque = atof(cfg.servo(id,"mintorque").c_str());
    maxTorque = atof(cfg.servo(id,"maxtorque").c_str());
    minAngle = atof(cfg.servo(id,"minangle").c_str());
    maxAngle = atof(cfg.servo(id,"maxangle").c_str());
    verbose = cfg.flag("servos.verbose",false);

    angle(0.0);
    speed(atof(cfg.servo(id,"minspeed").c_str()));
    torque(atof(cfg.servo(id,"mintorque").c_str()));
    tempRate(atof(cfg.servo(id,"rxtemprate").c_str()));
    positionRate(atof(cfg.servo(id,"rxpositionrate").c_str()));
    presentTemp = 0;
    curveMode = false;
#if USE_BROADCAST != 1
    update();
#endif
  }
  
  uint8_t temp() const
  {
    return presentTemp;
  }

  void curve(double t_[2], float c0_[3],float c1_[3])
  {
    curveMode = true;
    t[0]=t_[0];
    t[1]=t_[1];
    c0[0]=c0_[0];
    c0[1]=c0_[1];
    c0[2]=c0_[2];
    c1[0]=c1_[0];
    c1[1]=c1_[1];
    c1[2]=c1_[2];

//    if (verbose) cout << "dynamixel curve" << " servo=" << id << " t=[" << t[0] << "," << t[1] << "] c0=[" << c0[0] << "," << c0[1] << "," << c0[2] << "]" << " c1=[" << c1[0] << "," << c1[1] << "," << c1[2] << "]"  << endl;
  }

  float angle() const { 
    return (180.0/2048)*(presentPosition-2048); 
  }

  void angle0(float value) {
    if (value < minAngle) value = minAngle;
    else if (value > maxAngle) value = maxAngle;
    goalPosition = value*(2048/180.0)+2048;
    //    cout << "dynamixel servo=" << id << " goal position=" << goalPosition << endl;
  }
  void angle(float value) {
    curveMode = false;
    angle0(value);
  }

  void speed(float value) {
    // speed for MX-110t (not MX-28t)
    //    cout << "id=" << id << " set speed = " << value << endl;
    //    value = fabs(value) + 0.5*fabs(goalPosition-presentPosition);
    //    if (fabs(value) < minSpeed) {
    //      if (value < 0) value=-minSpeed;
    //      else value = minSpeed;
    //    }
    value = fabs(value);
    if (value < minSpeed) { value = minSpeed; }
    else if (value > maxSpeed) { value = maxSpeed; }
    goalSpeed = fabs(value)*(60.0/360.0)*(1023/117.07)*SPEED_FACTOR;
    if (goalSpeed > 480) goalSpeed = 480;
  }

  float speed() const {
    return presentSpeed/((60.0/360.0)*(1023/117.07));
  }

  void torque(float value) {
    //    cout << "id=" << id << " set torque = " << value << endl;
    value = fabs(value);
    if (0 < value && value < minTorque) {
      value = minTorque;
    } else if (value > maxTorque) {
      value = maxTorque;
    }
    goalTorque = fabs(value)*(1023);
    if (goalTorque > 1023) goalTorque = 1023;
  }

  float torque() const {
    return presentTorque/1023.0;
  }

  void rate(float value) {
    positionRate(value);
  }

  void positionRate(float value) {
    rxPositionRate = value;
    if (rxPositionRate > 0) {
      rxPositionTime = min(rxPositionTime,now()+1.0/rxPositionRate);
    }
  }

  void tempRate(float value) {
    rxTempRate = value;
    if (rxTempRate > 0) {
      rxTempTime = min(rxTempTime,now()+1.0/rxTempRate);
    }
  }

  void tx()
  {
    if (goalTorque != 0) {
      io.writeWord(id,DXL_GOAL_POSITION_WORD,(goalPosition & 4095));
      io.writeWord(id,DXL_MOVING_SPEED_WORD,goalSpeed);
      io.writeWord(id,DXL_TORQUE_WORD,goalTorque);
      io.writeWord(id,DXL_TORQUE_ENABLE_BYTE,1);
    } else {
      io.writeWord(id,DXL_TORQUE_ENABLE_BYTE,0);
    }
  }

  double rxTempTime;
  double rxPositionTime;

  void rx()
  {
    rxTemp();
    rxPosition();
  }

  void rxTemp()
  {
    if (rxTempRate == 0 || rxTempTime >= now()) return;

    uint8_t inp;
    if (io.readByte(id,DXL_PRESENT_TEMP_BYTE,&inp)) {
      presentTemp = inp;
    } else {
      cout << "comm rx temp error" << endl;
    }
    rxTempTime = now() + 1.0/rxTempRate;
  }

  void rxPosition()
  {
    if (rxPositionRate==0 || rxPositionTime > now()) return;

    int inp;
    if (io.readWord(id,DXL_PRESENT_POSITION_WORD,&inp)) {
      presentPosition = inp;
    } else {
      cout << "comm rx position error" << endl;
    }
    rxPositionTime = now() + 1.0/rxPositionRate;
  }

  void update()
  {
#if USE_BROADCAST == 0
    tx();
#endif
    rx();
  }

  ~DynamixelServo()
  {
    io.writeWord(id,DXL_TORQUE_WORD,1);
  }
};

struct DynamixelServoController : ServoController
{
  Configure &cfg;
  DXLIO io;
  typedef std::map < int , std::shared_ptr <DynamixelServo> > Servos;
  Servos servos;
  bool running;
  double txRate;
  Servo* servo(int id) {
    Servos::iterator i = servos.find(id);
    if (i != servos.end()) return &*i->second;
    assert(running == false);

    return &*(servos[id] = 
	      std::shared_ptr <DynamixelServo> (new DynamixelServo(cfg,io,id)));
  }

  int countServosInRange(int lower,int upper) {
    int retval=0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) retval++;
    }
    return retval;
  }

  void broadcastSpeedPosition(bool output,double t,double t1,int lower,int upper) {
    io.reopen(); // reopen if failing recently...
    int N = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) {
	if (k->second->enabled) {
	  ++N;
	}
      }
    }
    if (N == 0) return;

    int L = 4; // total data payload for position + speed
    
    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_GOAL_POSITION_WORD);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    //	std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    //	std::cout.precision(5);
    //	if (output) {
    //	  cout << endl;
    //	  cout << "t," << setprecision(15) << t << ",";
    //	}
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper)  {
	if (k->second->enabled) {
	  DynamixelServo *servo = &*k->second;
	  
	  if (servo->curveMode) {
	    double t0=servo->t[0];

	    double dt;
	    if (t < servo->t[1]) {
	      dt = t-servo->t[0];
	    } else {
	      dt = servo->t[1]-servo->t[0];
	      //	      cout << "servo " << id << " stale " << t - servo->t[1] << " seconds" << endl;
	    }
	    float angle;
	    float speed;
	    {
	      double dtsq = dt*dt;
	      float *c = (dt <= 0) ? servo->c0 : servo->c1;
#if USE_SERVO_LINEAR == 1
	      angle = c[0];
	      speed = c[1];

#else
	      angle = c[0]+c[1]*dt+c[2]*dtsq/2.0;
	      speed = c[1]+c[2]*dt;
#endif
	    }

	    double dt1;
	    if (t1 < servo->t[1]) {
	      dt1 = t-servo->t[0];
	    } else {
	      dt1 = servo->t[1]-servo->t[0];
	    }

	    float angle1;
	    float speed1;
	    {
	      double dt1sq = dt1*dt1;
	      float *c = (dt1 <= 0) ? servo->c0 : servo->c1;
#if USE_SERVO_LINEAR == 1
	      angle1 = c[0];
	      speed1 = c[1];
#else
	      angle1 = c[0]+c[1]*dt1+c[2]*dt1sq/2.0;
	      speed1 = c[1]+c[2]*dt1;
#endif
	    }

	    servo->angle0(angle1);
	    servo->speed(max(fabs(speed),fabs(speed1)));
	  }
	  int position = servo->goalPosition & 4095;
	  int speed = servo->goalSpeed;

	  //	  cout << "DXL PS," << t << "," << id << "," << position <<  "," << speed << endl;
	  
	  dxl_set_txpacket_parameter(i*(L+1)+2,id);
	  dxl_set_txpacket_parameter(i*(L+1)+3,dxl_get_lowbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+4,dxl_get_highbyte(position));
	  dxl_set_txpacket_parameter(i*(L+1)+5,dxl_get_lowbyte(speed));
	  dxl_set_txpacket_parameter(i*(L+1)+6,dxl_get_highbyte(speed));
	  ++i;
	}
      }
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 
    
    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }

  void broadcastTorque(bool output,double t,int lower,int upper) {
    io.reopen(); // reopen if failing recently...

    int N = countServosInRange(lower,upper);
    int L = 2; // total data payload for torque

    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_TORQUE_WORD);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper) {
	DynamixelServo *servo = &*k->second;
	//	int torque = 1023; // servo->goalTorque;  
	int torque = servo->goalTorque;  
	dxl_set_txpacket_parameter(i*(L+1)+2,id);
	dxl_set_txpacket_parameter(i*(L+1)+3,dxl_get_lowbyte(torque));
	dxl_set_txpacket_parameter(i*(L+1)+4,dxl_get_highbyte(torque));
	//	cout << "DXL T," << t << "," << id << "," << torque << endl;
	++i;
      }
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 

    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }

  void broadcastTorqueEnable(bool output,double t,int lower,int upper) {
    io.reopen(); // reopen if failing recently...

    // count servos with changed enable state...
    int N = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      if (k->first>=lower && k->first<=upper) {
	if (!k->second->enabled || (k->second->goalTorque == 0)) {
	  ++N;
	}
      }
    }

    if (N == 0) return;
	

    int L = 1; // total data payload for torque

    dxl_set_txpacket_id(BROADCAST_ID);
    dxl_set_txpacket_instruction(INST_SYNC_WRITE);
    dxl_set_txpacket_parameter(0, DXL_TORQUE_ENABLE_BYTE);
    dxl_set_txpacket_parameter(1, L);   // L bytes sent to each Dynamixel 
    dxl_set_txpacket_length((L+1)*N+4); // bytes in packet exc. Header
    int i = 0;
    for (Servos::iterator k = servos.begin(); k != servos.end(); ++k) {
      int id = k->first;
      if (id>=lower && id<=upper) {
	if (!k->second->enabled || (k->second->goalTorque == 0)) {
	  DynamixelServo *servo = &*k->second;
	  int torque = servo->goalTorque;
	  dxl_set_txpacket_parameter(i*(L+1)+2,id);
	  dxl_set_txpacket_parameter(i*(L+1)+3,(torque != 0));
	  k->second->enabled = (torque != 0);
	  ++i;
	}
      }
    }
    //	cout << "Packet " << endl;
    dxl_txrx_packet();
    int result = dxl_get_result(); 

    if (result == COMM_RXSUCCESS) {
      io.okSince = now();
    }
  }
  // Packets are really limited to about 20 servos for three registers.  
  void update() {
    while (running) {
      for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	i->second->update();
      }

      double t = now();
      double t1=t+1.0/txRate;

      for (Servos::iterator i = servos.begin(); i != servos.end(); ++i) {
	if (i->second->curveMode) {
	  if (t < i->second->t[0] && t1 > i->second->t[0]+0.0001) {
	    t1=i->second->t[0]+0.0001;
	  }
	}
      }

      bool output =(floor(t) != floor(t1));
#if USE_BROADCAST
      {

	//	This makes the servos "tick" everytime it is sent
#if USE_TORQUE_ENABLED
#warning torque enable code enabled
	broadcastTorqueEnable(output,t1,0,49); // legs
	broadcastTorqueEnable(output,t1,90,99); // head/waist
	broadcastTorqueEnable(output,t1,50,59); // Left arms
	broadcastTorqueEnable(output,t1,60,69); // arms
#endif



	broadcastTorque(output,t1,0,49); // legs
	broadcastTorque(output,t1,90,99); // head/waist
	broadcastTorque(output,t1,50,59); // Left arms
	broadcastTorque(output,t1,60,69); // arms


	broadcastSpeedPosition(output,t,t1,0,49);
	broadcastSpeedPosition(output,t,t1,90,99);
	broadcastSpeedPosition(output,t,t1,50,59);
	broadcastSpeedPosition(output,t,t1,60,69);
      }
#endif
      t=now();
      if (t1 > t) {
	usleep(int((t1-t)*1000000));
      }
    }
  }

  void iThread() {
	bool dataNeeded;
	int rc, j, size = 34 * 2, sleep_time = 50;
	uint8_t msgArr[size];

	void* context = zmq_ctx_new ();
	void* rep = zmq_socket(context, ZMQ_REP);
	rc = zmq_bind(rep, "tcp://*:9001");
	assert(rc == 0);

	while(running) {
		int rv = zmq_recv(rep, &dataNeeded, sizeof(bool), ZMQ_DONTWAIT);
		j = 0;		

		if(dataNeeded) {
		//Get temps and populate array
			for (Servos::iterator i=servos.begin(); i != servos.end(); ++i) {
				msgArr[j++] = (uint8_t)i->first; //Servo ID
				msgArr[j] = (uint8_t)i->second->temp(); //Servo temp
			}

			zmq_msg_t msg;
			int rc = zmq_msg_init_size(&msg, size);
			memcpy(zmq_msg_data(&msg), msgArr, size);
			if(rc == 0)
			{
				int rc = zmq_sendmsg(rep, &msg, ZMQ_DONTWAIT);
			}
			zmq_msg_close(&msg);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	zmq_close(rep);
	zmq_ctx_destroy(context);
  }

  std::thread *go;
  std::thread *infoThread;

  void start()
  {
    if (!running) {
      running = true;
      go = new thread(&DynamixelServoController::update,this);
  //    infoThread = new thread(&DynamixelServoController::iThread,this);
    }
  }

  DynamixelServoController(Configure &cfg_, int deviceIndex, int baudNum)
    : cfg(cfg_), io(deviceIndex,baudNum)
  {
    running = false;
    txRate=cfg.num("servos.dynamixel.rate.tx");
  }

  ~DynamixelServoController()
  {
    if (running) {
      running = false;
      go->join();
      infoThread->join();
      delete go;
      delete infoThread;
   }
  }
};

ServoController* CreateDynamixelServoController(Configure &cfg, int deviceIndex,int baudNum)
{
  return new DynamixelServoController(cfg, deviceIndex,baudNum);
}
