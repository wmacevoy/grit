#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>
#include <mutex>
#include <zmq.h>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include "Configure.h"
#include "SensorsMessage.h"
#include "now.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

Configure cfg;
bool verbose;
volatile int die = 0;

void *context=0;
void *pub=0;

SensorsMessage sensors;
double okRead;
double readTimeout;
double okReadTimeout;
double okWrite;
double okWriteTimeout;

#define N 4096
char buffer[N];

int fd=-1;
string line;

void close_write()
{
  if (pub != 0) {
    zmq_close(pub);
    zmq_ctx_destroy(context);
    pub = 0;
  }
}

void open_write()
{
  close_write();

  int rc = 0;
  int hwm = 1;
  
  context = zmq_ctx_new ();
  pub = zmq_socket(context,ZMQ_PUB);
  rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
  
  rc = zmq_bind(pub, cfg.str("sensors.provider.publish").c_str());
  if (rc!=0) {
    int en=zmq_errno();
    std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
    close_write();
  }

  okWrite = now();
}

void write()
{
  if (pub == 0 || okWrite+okWriteTimeout < now()) {
    open_write();
  }

  if (pub != 0) {
    int rc = zmq_send(pub,&sensors, sizeof(SensorsMessage), ZMQ_DONTWAIT);
    if (rc!=0) {
      int en=zmq_errno();
      std::cout << "TCP Error Number " << en << " " << zmq_strerror(en) << std::endl;
    } else {
      okWrite = sensors.t;
    }
  }
}

void process(const std::string &line)
{
  istringstream iss(line);
  sensors.t = now();

  iss >> sensors.a[0] >> sensors.a[1] >> sensors.a[2];
  iss >> sensors.g[0] >> sensors.g[1] >> sensors.g[2];
  iss >> sensors.p[0] >> sensors.p[1] >> sensors.p[2] >> sensors.p[3];
  
  if (iss) {
    write();
    okRead=sensors.t;
  }
}

void process(char c)
{
  if (c == '\n') {
    if (line.length() > 0) process(line);
    line="";
  } else if (c >= ' ') {
    line.push_back(c);
  }
}

void process(ssize_t n, char *c)
{
  for (ssize_t i = 0; i<n; ++i) {
    process(c[i]);
  }
}

 void read_close()
 {
    if (fd >= 0) close(fd);
    fd = -1;
 }

 void read_open()
 {
   read_close();
   fd = open(cfg.str("sensors.publish.dev").c_str(),O_RDONLY);   
 }

void read()
{
  if (fd < 0 || okRead+okReadTimeout < now()) {
    read_open();
  }

  timespec ts;
  ts.tv_sec=0;
  ts.tv_nsec = readTimeout*1e9;

  fd_set set;
  int rv;

  FD_ZERO(&set); /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  rv = pselect(fd + 1, &set, NULL, NULL, &ts, NULL);
  if (rv > 0) {
    ssize_t n = read(fd,buffer,N);
    if (n > 0) process(n,buffer);
  }
}

void quit(int sig)
{
  die = 1;
}

int main(int argc, char** argv)
{
  cfg.path("../../setup");
  cfg.args("sensors.provider.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("sensors.provider.verbose", false);
  if (verbose) cfg.show();

  readTimeout = cfg.num("sensors.provider.readtimeout",0.25);
  okReadTimeout = cfg.num("sensors.provider.okreadtimeout",1.0);
  okWriteTimeout = cfg.num("sensors.provider.okwritetimeout",1.0);
  
  int hwm = 1;
  int rc = 0;
  
  
  signal(SIGINT, quit);
  signal(SIGQUIT, quit);
  
  while(!die) { 
    read();
  }

  if (fd >= 0) close(fd);
  
  zmq_close(pub);
  zmq_ctx_destroy(context);
  
  return 0;
}
