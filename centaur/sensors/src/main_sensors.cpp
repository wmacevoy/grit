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
#include "CSVSplit.h"

using namespace std;

Configure cfg;
bool verbose;
volatile bool running = true;

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

void write_close()
{
  if (pub != 0) {
    zmq_close(pub);
    pub = 0;
  }
}

void zmqerror()
{
  int en=zmq_errno();
  cout << "ZMQ error " 
       << zmq_strerror(en) 
       << " (" << en << ")" << endl;
}

void write_open()
{
  write_close();

  int rc = 0;
  int hwm = 1;
  
  pub = zmq_socket(context,ZMQ_PUB);
  rc = zmq_setsockopt(pub, ZMQ_SNDHWM, &hwm, sizeof(hwm));
  if (rc != 0) {
    zmqerror();
    write_close();
  } else {
    rc = zmq_bind(pub, cfg.str("sensors.provider.publish").c_str());
    if (rc!=0) {
      zmqerror();
      write_close();
    }
  }

  okWrite = now();
}

void write()
{
  if (pub == 0 || okWrite+okWriteTimeout < now()) {
    write_open();
  }

  if (pub != 0) {
    int rc = zmq_send(pub,&sensors, sizeof(SensorsMessage), ZMQ_DONTWAIT);
    if (rc!=0) {
    } else {
      okWrite = sensors.t;
    }
  }
}

void report()
{
  cout << "t=" << sensors.t;

  cout << " a=[" 
       << sensors.a[0] << ","
       << sensors.a[1] << ","
       << sensors.a[2] << "]";
  
  cout << " c=[" 
       << sensors.c[0] << ","
       << sensors.c[1] << ","
       << sensors.c[2] << "]";
  
  cout << " g=[" 
       << sensors.g[0] << ","
       << sensors.g[1] << ","
       << sensors.g[2] << "]";
  
  cout << " p=[" 
       << sensors.p[0] << ","
       << sensors.p[1] << ","
       << sensors.p[2] << ","
       << sensors.p[3] << "]";
  
  cout << endl;
}

void process(const std::string &line)
{
  sensors.t = now();

  vector<string> vals;
  CSVSplit(line,vals);
  if (vals.size() == 17) {
    bool ok = true;
    int i = -1;

    ok = ok && (vals[++i] == "A");
    sensors.a[0]=atof(vals[++i].c_str());
    sensors.a[1]=atof(vals[++i].c_str());
    sensors.a[2]=atof(vals[++i].c_str());

    ok = ok && (vals[++i] == "C");
    sensors.c[0]=atof(vals[++i].c_str());
    sensors.c[1]=atof(vals[++i].c_str());
    sensors.c[2]=atof(vals[++i].c_str());

    ok = ok && (vals[++i] == "G");
    sensors.g[0]=atof(vals[++i].c_str());
    sensors.g[1]=atof(vals[++i].c_str());
    sensors.g[2]=atof(vals[++i].c_str());

    ok = ok && (vals[++i] == "L");
    sensors.p[0]=atof(vals[++i].c_str());
    sensors.p[1]=atof(vals[++i].c_str());
    sensors.p[2]=atof(vals[++i].c_str());
    sensors.p[3]=atof(vals[++i].c_str());

    if (ok) {
      if (verbose) { 
	report();
      }
      okRead = sensors.t;
      write();
    }
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
  fd = open(cfg.str("sensors.provider.dev_path").c_str(),O_RDONLY|O_NONBLOCK);
  if (verbose) {
    cout << "open(" << cfg.str("sensors.provider.dev_path") << ")=" << fd << endl;
  }
  okRead = now();
}

ssize_t read_timeout(int fd, char *buffer, size_t n, size_t msTimeout)
{
  struct timespec ts;
  ts.tv_sec = msTimeout/1000;
  ts.tv_nsec = (msTimeout % 1000)*1000000;

  fd_set set;
  int rv;

  FD_ZERO(&set);
  FD_SET(fd, &set);

  rv = pselect(fd + 1, &set, NULL, NULL, &ts, NULL);

  if (rv > 0) {
    return read(fd,buffer,n);
  } else {
    return 0;
  }
}

void read()
{
  if (fd < 0 || okRead+okReadTimeout < now()) {
    read_open();
  }

  ssize_t n = read_timeout(fd,buffer,N, 1000*readTimeout);
  if (n > 0) process(n,buffer);
}

void quit(int sig)
{
  running = false;
}

void config(int argc, char** argv)
{
  cfg.path("../../setup");
  cfg.args("sensors.provider.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("sensors.provider.verbose", false);
  if (verbose) cfg.show();

  readTimeout = cfg.num("sensors.provider.readtimeout");
  okReadTimeout = cfg.num("sensors.provider.okreadtimeout");
  okWriteTimeout = cfg.num("sensors.provider.okwritetimeout");
}

int main(int argc, char** argv)
{
  config(argc,argv);

  signal(SIGINT, quit);
  signal(SIGQUIT, quit);

  context = zmq_ctx_new ();


  while(running) { 
    read();
  }

  read_close();
  write_close();

  zmq_ctx_destroy(context);
  
  return 0;
}
