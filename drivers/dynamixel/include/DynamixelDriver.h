/*
 * DynamixelDriver.hpp
 *
 *  Created on: Apr 15, 2013
 *      Author: kcastlet
 */

#ifndef DYNAMIXELDRIVER_HPP_
#define DYNAMIXELDRIVER_HPP_

#include <string>

using namespace std;

#define DXL_MODEL_WORD_BYTE      0
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

void DXL_PrintCommStatus(int CommStatus);

void DXL_PrintErrorCode();

class DXL_ComError:public exception {
	int status,line,id;
	string file;
public:
	static bool isOK(int status);
	DXL_ComError(int newStatus=-1,int newId=0,string fname="",int linenumber=0) ;
	void describe();
	~DXL_ComError() throw();
};

class DynamixelInterface {
	  static const int DEVICEINDEX;
	  static const int BAUDNUM;
	  bool ok;
public:
	  static const int JITTER; // How much will the position jitter
	  static const int RETRIES;
	  DynamixelInterface();
	  bool isOk() ;
	  ~DynamixelInterface() ;
	  void sendWord(int id,int address,int word) ;
	  void sendByte(int id,int address,unsigned char byte) ;
	  int readWord(int id,int address);
	  unsigned char readByte(int id,int address);
};

extern DynamixelInterface DXL2USB;

class DynamixelServo {
  int id;
  bool wheelMode;
  float presentAngle;
  string name;
public:
  float getAngle() ;
  void init(int newId,string newName) ;
  void wheel(int speed);
  void setTorque(int value=255);
  void joint(int position);
  void report ();
  void angle(float newAngle);
};

#endif /* DYNAMIXELDRIVER_HPP_ */
