/*
 * DynamixelDriver.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: kcastlet
 */

#include <iostream>
#include <unistd.h>

#include <dynamixel.h>

#include "DynamixelDriver.h"

void DXL_PrintCommStatus(int CommStatus)
{
  switch(CommStatus) {
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
    cerr << "This is unknown error code!" << CommStatus << endl;
    break;
  }
}

void DXL_PrintErrorCode()
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

DynamixelInterface::DynamixelInterface() {
  int status=dxl_initialize(DEVICEINDEX,BAUDNUM) == 0;
	//	  cout << status << endl;
		  ok=DXL_ComError::isOK(status);
	  }
bool DynamixelInterface::isOk() {
		  return ok;
	  }
DynamixelInterface::~DynamixelInterface() {
		  dxl_terminate();
	  }
void DynamixelInterface::sendWord(int id,int address,int word)  {
	  if (!DXL2USB.isOk()) throw DXL_ComError(-1,id,__FILE__,__LINE__);
	  int retry=DynamixelInterface::RETRIES;
	  int status=0;
	  do {
	    dxl_write_word(id,address,word);
	    retry--;
	    status=dxl_get_result();
	    //		    cout << status << endl;
	    if (!DXL_ComError::isOK(status)) usleep(10000);
	  }while (!DXL_ComError::isOK(status) && retry>0);
	  if (!DXL_ComError::isOK(status)) throw DXL_ComError(status,0,__FILE__,__LINE__);
}
void DynamixelInterface::sendByte(int id,int address,unsigned char byte) {
	  if (!DXL2USB.isOk()) throw DXL_ComError(-1,id,__FILE__,__LINE__);
	  int retry=DynamixelInterface::RETRIES;
	  int status=0;
	  do {
		  dxl_write_byte(id,address,byte);
		  retry--;
		    status=dxl_get_result();
		    //		    cout << status << endl;
		    if (!DXL_ComError::isOK(status)) usleep(10000);
	  }while (!DXL_ComError::isOK(status) && retry>0);
    if (!DXL_ComError::isOK(status)) throw DXL_ComError(status,id,__FILE__,__LINE__);
}
int DynamixelInterface::readWord(int id,int address) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1,id,__FILE__,__LINE__);
	  int retry=DynamixelInterface::RETRIES;
	  int status=0;
	  int value=0;
	  do {
		  value=dxl_read_word(id,address);
		  retry--;
		    status=dxl_get_result();
	//	    cout << status << endl;
		    if (!DXL_ComError::isOK(status)) {
		//		dxl_terminate();
				usleep(10000);
		//		dxl_initialize(DEVICEINDEX,BAUDNUM);
			}
	  }while (!DXL_ComError::isOK(status) && retry>0);
    if (!DXL_ComError::isOK(status)) {
    	unsigned char b=dxl_read_byte(id,DXL_ALARM_SHUTDOWN_BYTE);
    	cout << "Alarm Shutdown Byte:"<<((int)b)<<endl;
    	throw DXL_ComError(status,id,__FILE__,__LINE__);
    }
    return value;
}
unsigned char DynamixelInterface::readByte(int id,int address) {
	if (!DXL2USB.isOk())throw DXL_ComError(-1,id,__FILE__,__LINE__);
	int retry=DynamixelInterface::RETRIES;
	int status=0;
	unsigned char value=0;
	do {
		value=dxl_read_byte(id,address);
	    retry--;
	    status=dxl_get_result();
	    if (!DXL_ComError::isOK(status)) usleep(10000);
   }while (!DXL_ComError::isOK(status) && retry>0);
  if (!DXL_ComError::isOK(status)){
  	  unsigned char b=dxl_read_byte(id,DXL_ALARM_SHUTDOWN_BYTE);
  	  cout << "Alarm Shutdown Byte:"<<((int)b)<<endl;
	  throw DXL_ComError(status,id,__FILE__,__LINE__);
  }
  return value;
}

const int DynamixelInterface::DEVICEINDEX=0;
const int DynamixelInterface::BAUDNUM=34;
const int DynamixelInterface::JITTER=1000;
const int DynamixelInterface::RETRIES=10;
DynamixelInterface DXL2USB;

  float DynamixelServo::angle() const {
    return 180.0/2048*(DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD)-2048);
  }
  void DynamixelServo::init(int newId,string newName) {
	  if (!DXL2USB.isOk()) throw DXL_ComError(-1,id,__FILE__,__LINE__);
	  id=newId;
	  name=newName;
	  int mode=DXL2USB.readWord(id,DXL_CCW_ANGLE_LIMIT_WORD);
	  wheelMode=(mode==0);
	  //	  presentAngle=(DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD)-2048.0)*180.0/4096.0;
  }
  void DynamixelServo::wheel(int speed){
	 if (!wheelMode){
//		 cout << "w" <<endl;
		 DXL2USB.sendWord(id,DXL_CCW_ANGLE_LIMIT_WORD,0);
		 wheelMode=true;
	 }
	 DXL2USB.sendWord(id,DXL_MOVING_SPEED_WORD,speed);
  }
  void DynamixelServo::setTorque(int value){
	//  sendWord(DXL_MAX_TORQUE_WORD,value);
	  DXL2USB.sendWord(id,DXL_TORQUE_WORD,value);
	  usleep(100);
	  DXL2USB.sendByte(id,DXL_TORQUE_MODE_BYTE,0);
	  usleep(100);
	  DXL2USB.sendWord(id,DXL_MOVING_SPEED_WORD,150);
	  usleep(100);
  }
  void DynamixelServo::joint(int position){
	  if(wheelMode){
//		  cout << "j"<<position<<" "<<endl;
		  DXL2USB.sendWord(id,DXL_CCW_ANGLE_LIMIT_WORD,4095);
		  wheelMode=false;
	  }
	  DXL2USB.sendWord(id,DXL_GOAL_POSITION_WORD,position);
  }
  void DynamixelServo::report () {
	  int load=DXL2USB.readWord(id,DXL_PRESENT_LOAD_WORD);
	 cout << name << " ";
	 if ((load & 1024)!=0) cout << "cw  ";
	 else cout << "ccw ";
	 cout << (load & 1023) <<",";
  }
void DynamixelServo::angle(float newAngle) {
  joint(2048+2048/180.0*newAngle);
#if 0
	 int rev=0;
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
	 int pos=DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD);
	 while (rev>0){
	    wheel(1023);
	    int newpos=DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD);
	    if (pos-newpos>DynamixelInterface::JITTER) rev--;
	    pos=newpos;
	 }
	 while (rev<0){
		 wheel(1024+1023);
		 int newpos=DXL2USB.readWord(id,DXL_PRESENT_POSITION_WORD);
		 if (newpos-pos>DynamixelInterface::JITTER) rev++;
		 pos=newpos;
	 }
	 diffAngle=presentAngle;
	 while (diffAngle>180)diffAngle-=360.0;
	 while (diffAngle<-180) diffAngle+=360.0;
	 int angle=((180-diffAngle)*2047)/180;
	 joint(angle);
#endif
  }





