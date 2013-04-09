#include <iostream>
#include <stdio.h>
#include <dynamixel.h>
#include <termio.h>
#include <stdlib.h>

using namespace std;

#define DXL_MODEL	  return dxl_read_byte(id,address);_WORD_BYTE      0
#define DXL_FIRMWARE_BYTE        2
#define DXL_ID_BYTE              3
#define DXL_BAUD_BYTE            4
#define DXL_RETURN_DELAY_BYTE    5
#define DXL_CW_ANGLE_LIMIT_WORD  6
#define DXL_CCW_ANGLE_LIMIT_WORD 8
#define DXL_DRIVE_BYTE           10
#define DXL_HIGHEST_TEMP_BYTE    11
#define DXL_LOWEST_VOLT_BYTE     12
#define DXL_HIGHEST_VOLT_BYTE    13
#define DXL_MAX_TORQUE_WORD      14
#define DXL_STATUS_BYTE          16
#define DXL_ALARM_LED_BYTE       17
#define DXL_ALARM_SHUTDOWN_BYTE  18
#define DXL_TORQUE_ENABLE_BYTE   24
#define DXL_LED_BYTE             25
#define DXL_D_GAIN_BYTE          26
#define DXL_I_GAIN_BYTE          27
#define DXL_P_GAIN_BYTE          28
#define DXL_GOAL_POSITION_WORD   30
#define DXL_MOVING_SPEED_WORD    32
#define DXL_TORQUE_WORD          34
#define DXL_PRESENT_POSITION_WORD 36
#define DXL_PRESENT_SPEED_WORD   38
#define DXL_PRESENT_LOAD_WORD    40
#define DXL_PRESENT_VOLT_BYTE    42
#define DXL_PRESENT_TEMP_BYTE    43
#define DXL_REGISTERED_BYTE      44
#define DXL_MOVING_BYTE          46
#define DXL_LOCK_BYTE            47
#define DXL_PUNCH_WORD           48
#define DXL_CURRENT_WORD         68
#define DXL_TORQUE_MODE_BYTE     70
#define DXL_GOAL_TORQUE_WORD     71
#define DXL_GOAL_ACCEL_BYTE      73

#define DEFAULT_BAUDNUM		34
#define DEFAULT_ID		0

void PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		cerr <<"COMM_TXFAIL: Failed transmit instruction packet!"<<endl;
		break;

	case COMM_TXERROR:
		cerr << "COMM_TXERROR: Incorrect instruction packet!"<<endl;
		break;

	case COMM_RXFAIL:
		cerr<<"COMM_RXFAIL: Failed get status packet from device!"<<endl;
		break;

	case COMM_RXWAITING:
		cerr << "COMM_RXWAITING: Now recieving status packet!"<<endl;
		break;

	case COMM_RXTIMEOUT:
		cerr << "COMM_RXTIMEOUT: There is no status packet!"<<endl;
		break;

	case COMM_RXCORRUPT:
		cerr << "COMM_RXCORRUPT: Incorrect status packet!"<<endl;
		break;

	default:
		cerr << "This is unknown error code!" <<endl;
		break;
	}
}

void PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		cerr <<"Input voltage error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		cerr <<"Angle limit error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		cerr <<"Overheat error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		cerr <<"Out of range error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		cerr <<"Checksum error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		cerr <<"Overload error!"<<endl;
	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		cerr <<"Instruction code error!"<<endl;
}

class DXL_ComError:public exception {
	int status;
public:
	DXL_ComError(int newStatus=-1) {
		status=newStatus;
//	    cerr<< "Status "<<status<<endl;
	}
	bool isOK(){
		return status==COMM_RXSUCCESS || status==COMM_TXSUCCESS
				|| status==COMM_RXWAITING || status==COMM_RXTIMEOUT;
	}
};

class DynamixelInterface {
	  static const int DEVICEINDEX;
	  static const int BAUDNUM;
	  bool ok;
public:
	  static const int JITTER; // How much will the position jitter
	  DynamixelInterface() {
		  int status=dxl_initialize(DEVICEINDEX,BAUDNUM) == 0;
	//	  cout << status << endl;
		  ok=(status==0);
	  }
	  bool isOk() {
		  return ok;
	  }
	  ~DynamixelInterface() {
		  dxl_terminate();
	  }
};
const int DynamixelInterface::DEVICEINDEX=0;
const int DynamixelInterface::BAUDNUM=34;
const int DynamixelInterface::JITTER=1000;
DynamixelInterface DXL2USB;

class Servo {
  int id;
  bool wheelMode;
  float presentAngle;
public:
  float getAngle() {
	 return presentAngle;
  }
  void sendWord(int address,int word)  {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  dxl_write_word(id,address,word);
	  DXL_ComError ce2(dxl_get_result());
      if (!ce2.isOK())throw ce2;
  }
  void sendByte(int address,unsigned char byte) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  dxl_write_byte(id,address,byte);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
  }
  int readWord(int address) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  int value=dxl_read_word(id,address);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
      else return value;
  }
  unsigned char readByte(int address) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  unsigned char value=dxl_read_word(id,address);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
      else return value;
  }
  Servo(int newId) {
	  if (!DXL2USB.isOk())
		  throw DXL_ComError(-1);
	  id=newId;
	  int mode=readWord(DXL_CCW_ANGLE_LIMIT_WORD);
	  wheelMode=(mode==0);
	  presentAngle=(readWord(DXL_PRESENT_POSITION_WORD)-2048.0)*180.0/4096.0;
  }
  void wheel(int speed){
	 if (!wheelMode){
//		 cout << "w" <<endl;
		 sendWord(DXL_CCW_ANGLE_LIMIT_WORD,0);
		 wheelMode=true;
	 }
	 sendWord(DXL_MOVING_SPEED_WORD,speed);
  }
  void joint(int position){
	  if(wheelMode){
//		  cout << "j"<<position<<" "<<endl;
		  sendWord(DXL_CCW_ANGLE_LIMIT_WORD,4095);
		  wheelMode=false;
	  }
	  sendWord(DXL_GOAL_POSITION_WORD,position);
  }
  void angle(float newAngle) {
	 int rev=0;
//	 cout << "NewAngle:"<<newAngle<<endl;
//	 cout <<"PresentAngle:"<<presentAngle <<endl;
	 float diffAngle=newAngle-presentAngle;
	 presentAngle=newAngle;
	 while (diffAngle>180.0) {
		 rev--;
		 diffAngle-=360.0;
	 }
	 while(diffAngle<-180.0){
		 rev++;
		 diffAngle+=360.0;
     }
	 int pos=readWord(DXL_PRESENT_POSITION_WORD);
//	 cout << "Revs:"<<rev<<" diffAngle:"<<diffAngle<<" pos:"<<pos<<endl;
	 while (rev>0){
	    wheel(1023);
	    int newpos=readWord(DXL_PRESENT_POSITION_WORD);
//	    cout << "ccw:"<<pos <<"," << newpos << endl;
	    if (pos-newpos>DynamixelInterface::JITTER) rev--;
	    pos=newpos;
	 }
	 while (rev<0){
		 wheel(1024+1023);
		 int newpos=readWord(DXL_PRESENT_POSITION_WORD);
//		 cout << "cw:"<<pos <<"," << newpos << endl;
		 if (newpos-pos>DynamixelInterface::JITTER) rev++;
		 pos=newpos;
	 }
	 diffAngle=presentAngle;
	 while (diffAngle>180)diffAngle-=360.0;
	 while (diffAngle<-180) diffAngle+=360.0;
//	 cout <<"diffAngle:"<<diffAngle<<endl;
	 int angle=((180-diffAngle)*2047)/180;
/*	 int newpos=readWord(DXL_PRESENT_POSITION_WORD);
	 while (abs(newpos-angle)>1024){
		newpos=readWord(DXL_PRESENT_POSITION_WORD);
	 } */
	 joint(angle);
  }
};

int main()
{
	Servo a(12);
	Servo b(22);
	b.joint(2048);
	a.joint(2048);
	sleep(2);
	while(1)
	{
		cout <<"Press Enter key to continue!(press q and Enter to quit)"<<endl;
		if(getchar() == 'q')
			break;
		a.angle(-400);
		sleep(3);
		a.angle(0);
		sleep(3);
		a.angle(400);
		sleep(3);
	}
	a.joint(2048);
	b.joint(2048);
	return 0;
}