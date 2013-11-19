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
#include <condition_variable>

using namespace std;

Configure cfg;
bool verbose;

class Sensors
{
private:
  SensorsMessage data;
  mutable std::mutex accessMutex;

  void copyTo(SensorsMessage &dest) const
  {
    Lock lock(access);
    memcpy(&dest,&data,sizeof(SensorsMessage));
  }

  void copyFrom(const SensorsMessage &src)
  {
    Lock lock(access);
    memcpy(&data,&src,sizeof(SensorsMessage));
  }

  Sensors()
  {
    memset(&data,0,sizeof(SensorsMessage));
  }
};

Sensors current;

class ZMQIO : ZMQHub
{
  void init() {
    rxPollTimeout = cfg.num(cfg.num("sensors.rxpolltimeout"));
    rxTimeout = cfg.num(cfg.num("sensors.rxtimeout"));
    txTimeout = cfg.num(cfg.num("sensors.txtimeout"));
    subscribers = cfg.list("sensors.subscribers");
    publish = cfg.str("sensors.publish");
  }

  void txWait()
  {
    std::unique_lock<std::mutex> lock(readyMutex);
    for (;;) {
      if (ready || !running) break;
      readyCondition.wait_for(lock,1000/(2*rate));
    }
  }

  void tx(ZMQPublishSocket &socket)
  {
    ZMQMessage msg(sizeof(SensorsMessage));
    current.copyTo(msg.data());
    msg.send();
    ready=false;
  }

  void tx()
  {
    tx_ready.signal();
  }
};

     
void report()
{
  SensorsMessage sensors;
  current.copyTo(sensors);
  
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
  
  cout << " s=["
       << sensors.s[0] << ","
       << sensors.s[1] << "]";
  
  cout << endl;
}

class ArduinoIO
{
  float timeout;
  double ok;
  int fd;
  string line;
  std::thread *go;
  bool running = false;
  float rate;

  ArduinoIO()
  {
    rxReady=false;
    go=0;
  }

  void init()
  {
    rxReady=false;
    go=0;
    rate=cfg.num("sensors.rate");
  }

  void start()
  {
    if (go == 0) {
      go = new std::thread(ArduinoIO::update,this);
    }
  }

  void stop()
  {
    if (go != 0) {
      running = false;
      go->join();
      delete go;
      go = 0;
    }
  }

  void close()
  {
    if (fd >= 0) close(fd);
    fd = -1;
  }

  void open()
  {
    close();
    fd = open(cfg.str("sensors.dev_path").c_str(),O_NONBLOCK);
    if (verbose) {
      cout << "open(" << cfg.str("sensors.dev_path") << ")=" << fd << endl;
    }
    ok=now();
  }

  void write()
  {
    SensorMessage copy;
    current.copyTo(copy);
    char tmp[32];
    snprintf(sizeof(tmp),tmp,"S0=%d\n",copy.s[0]);
    write(fd,tmp,sizeof(tmp));
  }

  void process(const std::string &line)
  {
    SensorsMessage sesnors;

    sensors.t = now();
    
    vector<string> vals;
    CSVSplit(line,vals);
    if (vals.size() == 20) {
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
      
      ok = ok && (vals[++i] == "S");
      sensors.f[0]=atoi(vals[++i].c_str());
      sensors.f[1]=atoi(vals[++i].c_str());
      
      if (ok) {
	current.copyFrom(sensors);
	if (zmqSensors.txSocket) {
	  zmqSensors.tx(*zmqSensors.txSocket);
	}
	if (verbose) { 
	  report();
	}
	okRead = sensors.t;
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
      return ::read(fd,buffer,n);
    } else {
      return 0;
    }
  }

  void read()
  {
    write();
    char tmp[80];
    size_t n=read_timeout(fd,tmp,sizeof(tmp),size_t(1000/(2*rate)));
    if (n > 0) process(n,tmp);
  }

  void run()
  {
    open();
    double txTime = now() + 1.0/rate;
    while (running) {
      if (txTime < now()) {
	write();
	txTime += 1.0/rate;
      }
      read();
    }
    close();
  }
};

ArduinoIO arduinoIO;


ZMQIO zmqIO;

void quit(int sig)
{
  zmqSensors->stop();
  zmqSensors->join();
  arduinoIO->stop();
}

void config(int argc, char** argv)
{
  cfg.path("../../setup");
  cfg.args("sensors.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("sensors.verbose", false);
  if (verbose) cfg.show();
}

int main(int argc, char** argv)
{
  config(argc,argv);

  signal(SIGINT, quit);
  signal(SIGTERM, quit);
  signal(SIGQUIT, quit);

  arduinoIO.start();
  zmqIO.start();
  zmqIO.join();
  
  return 0;
}
