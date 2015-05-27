#pragma once

#include <string>
#include <sys/time.h>

#define GRIT_POSITION_BYTE              3

struct GritIO
{
  std::string dev;
  int deviceIndex;
  int baud;
  int baudNum;

  int fd;
  double okSince;
  struct timespec timeout;
  fd_set fds;
  GritIO(int deviceIndex_=0, int baudNum_=34);
  GritIO(const char *dev_, size_t baud_);

  void reopen();
  void open();
  void close();
  bool write(ssize_t size, const uint8_t *data);
  ssize_t read0(size_t size, uint8_t *data);
  bool read(ssize_t size, unsigned char *data);
  ~GritIO();
};

