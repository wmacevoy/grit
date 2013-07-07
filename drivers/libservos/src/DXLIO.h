#pragma once


#include <string>

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

#define DXL_DEFAULT_BAUDNUM	 34
#define DXL_DEFAULT_ID	          0
#define DXL_DEFAULT_DEV          "/dev/ttyUSB1"

void DXL_PrintCommStatus(int CommStatus);
void DXL_PrintErrorCode();

struct DXLIO
{
  std::string dev;
  int baud;

  int fd;
  double okSince;
  void reopen();
  DXLIO(const char *dev_, size_t baud_);
  ~DXLIO();
  bool write(ssize_t size, const unsigned char *data);
  ssize_t read0(size_t size, unsigned char *data);
  bool read(ssize_t size, unsigned char *data);
  bool write_word(int id, int address, unsigned value);
  bool write_byte(int id, int address, unsigned value);
  int read_word(int id, int address);
  int read_byte(int id, int address);
};
