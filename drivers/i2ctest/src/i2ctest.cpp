#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

#pragma pack(1)

#define crc uint8_t

using namespace std;

// setup
const char *i2cPath = "/dev/i2c-2";

//CRC stuffs
crc crcTable[256];
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

void crcInit();

crc crcFast(const crc message[], int nBytes);

typedef struct {
  uint8_t i2c_address;
  struct {
    uint8_t config_address;
    uint16_t value;
    crc checksum;
  } message;
} packet_t;

// write buffer out to wire
int i2cWrite(packet_t *packet, int handle) {
  ioctl(handle, I2C_SLAVE, packet->i2c_address);
  packet->message.checksum = crcFast((uint8_t*)&(packet->message), 3);
  ssize_t status=write(handle, &packet->message, sizeof(packet->message));
  cout << "i2c_address=" << (unsigned) packet->i2c_address << "config_address=" << (unsigned) packet->message.config_address << " value=" << packet->message.value << " crc=" << (unsigned) packet->message.checksum << " status=" << status << endl;
  return status;
}

int main(int argc, char **argv) {
  crcInit();

  packet_t packet;
  packet.i2c_address=atoi(argv[1]);
  //cout << address << endl;
  int result = 0;
	
  int i2cHandle = open(i2cPath, O_RDWR);
  if (i2cHandle < 0) {
    cout << "Can't open i2c device" << endl;
    return 1;
  }
  result = ioctl(i2cHandle, I2C_TENBIT, 0);
  packet.message.config_address = (uint8_t)atoi(argv[2]);
  //cout << packet->data.address << endl;
  packet.message.value = atoi(argv[3]);
  //cout << packet->data.value << endl;
  result = i2cWrite(&packet, i2cHandle);
  if (result != 1){
    cout << "No ACK bit" << endl;
    return 1;
  }
  return 0;
}

//CRC lookupTable
void
crcInit(void)
{
  crc remainder;


  /*
   * Compute the remainder of each possible dividend.
   */
  for (int dividend = 0; dividend < 256; ++dividend)
    {
      /*
       * Start with the dividend followed by zeros.
       */
      remainder = dividend << (WIDTH - 8);

      /*
       * Perform modulo-2 division, a bit at a time.
       */
      for (uint8_t bit = 8; bit > 0; --bit)
        {
	  /*
	   * Try to divide the current data bit.
	   */			
	  if (remainder & TOPBIT)
            {
	      remainder = (remainder << 1) ^ POLYNOMIAL;
            }
	  else
            {
	      remainder = (remainder << 1);
            }
        }

      /*
       * Store the result into the table.
       */
      crcTable[dividend] = remainder;
    }

}   /* crcInit() */

crc
crcFast(uint8_t const message[], int nBytes)
{
  crc data;
  crc remainder = 0;


  /*
   * Divide the message by the polynomial, a byte at a time.
   */
  for (int byte = 0; byte < nBytes; ++byte)
    {
      data = message[byte] ^ (remainder >> (WIDTH - 8));
      remainder = crcTable[data] ^ (remainder << 8);
    }

  /*
   * The final remainder is the CRC.
   */
  return (remainder);

}   /* crcFast() */
