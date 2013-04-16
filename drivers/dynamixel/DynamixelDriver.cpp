/*
 * DynamixelDriver.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: kcastlet
 */

#include <iostream>

#include <dynamixel.h>

#include "DynamixelDriver.hpp"

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


 DXL_ComError::DXL_ComError(int newStatus) {
		status=newStatus;
//	    cerr<< "Status "<<status<<endl;
	}
bool DXL_ComError::isOK(){
		return status==COMM_RXSUCCESS || status==COMM_TXSUCCESS
				|| status==COMM_RXWAITING || status==COMM_RXTIMEOUT;
	}


DynamixelInterface::DynamixelInterface() {
		  int status=dxl_initialize(DEVICEINDEX,BAUDNUM) == 0;
	//	  cout << status << endl;
		  ok=(status==0);
	  }
bool DynamixelInterface::isOk() {
		  return ok;
	  }
DynamixelInterface::~DynamixelInterface() {
		  dxl_terminate();
	  }
const int DynamixelInterface::DEVICEINDEX=0;
const int DynamixelInterface::BAUDNUM=34;
const int DynamixelInterface::JITTER=1000;
DynamixelInterface DXL2USB;


  float Servo::getAngle() {
	 return presentAngle;
  }
  void Servo::sendWord(int address,int word)  {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  dxl_write_word(id,address,word);
	  DXL_ComError ce2(dxl_get_result());
      if (!ce2.isOK())throw ce2;
  }
  void Servo::sendByte(int address,unsigned char byte) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  dxl_write_byte(id,address,byte);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
  }
  int Servo::readWord(int address) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  int value=dxl_read_word(id,address);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
      else return value;
  }
  unsigned char Servo::readByte(int address) {
	  if (!DXL2USB.isOk())throw DXL_ComError(-1);
	  unsigned char value=dxl_read_word(id,address);
	  DXL_ComError ce(dxl_get_result());
      if (!ce.isOK())throw ce;
      else return value;
  }
  void Servo::init(int newId,string newName) {
	  if (!DXL2USB.isOk())
		  throw DXL_ComError(-1);
	  id=newId;
	  name=newName;
	  int mode=readWord(DXL_CCW_ANGLE_LIMIT_WORD);
	  wheelMode=(mode==0);
	  presentAngle=(readWord(DXL_PRESENT_POSITION_WORD)-2048.0)*180.0/4096.0;
  }
  void Servo::wheel(int speed){
	 if (!wheelMode){
//		 cout << "w" <<endl;
		 sendWord(DXL_CCW_ANGLE_LIMIT_WORD,0);
		 wheelMode=true;
	 }
	 sendWord(DXL_MOVING_SPEED_WORD,speed);
  }
  void Servo::setTorque(int value){
	//  sendWord(DXL_MAX_TORQUE_WORD,value);
	  sendWord(DXL_TORQUE_WORD,value);
	  sendByte(DXL_TORQUE_MODE_BYTE,0);
	  sendWord(DXL_MOVING_SPEED_WORD,150);
  }
  void Servo::joint(int position){
	  if(wheelMode){
//		  cout << "j"<<position<<" "<<endl;
		  sendWord(DXL_CCW_ANGLE_LIMIT_WORD,4095);
		  wheelMode=false;
	  }
	  sendWord(DXL_GOAL_POSITION_WORD,position);
  }
  void Servo::report () {
	  int load=readWord(DXL_PRESENT_LOAD_WORD);
	 cout << name << " ";
	 if ((load & 1024)!=0) cout << "cw  ";
	 else cout << "ccw ";
	 cout << (load & 1023) <<",";
  }
  void Servo::angle(float newAngle) {
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
	 int pos=readWord(DXL_PRESENT_POSITION_WORD);
	 while (rev>0){
	    wheel(1023);
	    int newpos=readWord(DXL_PRESENT_POSITION_WORD);
	    if (pos-newpos>DynamixelInterface::JITTER) rev--;
	    pos=newpos;
	 }
	 while (rev<0){
		 wheel(1024+1023);
		 int newpos=readWord(DXL_PRESENT_POSITION_WORD);
		 if (newpos-pos>DynamixelInterface::JITTER) rev++;
		 pos=newpos;
	 }
	 diffAngle=presentAngle;
	 while (diffAngle>180)diffAngle-=360.0;
	 while (diffAngle<-180) diffAngle+=360.0;
	 int angle=((180-diffAngle)*2047)/180;
/*	 int newpos=readWord(DXL_PRESENT_POSITION_WORD);
	 while (abs(newpos-angle)>256){
		newpos=readWord(DXL_PRESENT_POSITION_WORD);
	 }*/
	 joint(angle);
  }





