#include <iostream>
#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <string>
#include <string.h>
#include <csignal>
#include <signal.h>
#include <assert.h>

#include "config.h"
#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"

using namespace std;

struct Lock
{
  std::mutex &m;
  Lock(std::mutex &m_) : m(m_) { m.lock(); }
  ~Lock() { m.unlock(); }
};

class Commander : public ZMQHub
{
public:
  mutex sendsMutex;
  list < string > sends;
  
  void send(const string &content)
  {
    Lock lock(sendsMutex);
    sends.push_back(content);
  }

  void rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    msg.recv(socket);
    char *data = (char*) msg.data();
    string reply(data+1,data[0]);
    cout << "reply: " << reply << endl;
  }

  void tx(ZMQPublishSocket &socket)
  {
    Lock lock(sendsMutex);

    while (!sends.empty()) {
      string &message = *sends.begin();
      uint8_t size = (message.size() < BODY_MESSAGE_MAXLEN) ? message.size() : BODY_MESSAGE_MAXLEN;
      ZMQMessage msg(size+1);
      char *data = (char*)msg.data();
      data[0]=size;
      memcpy(data+1,&message[0],size);
      msg.send(socket);
      sends.pop_front();
    }
  }
};

Commander *pserver=0;

void SigIntHandler(int arg) {
  pserver->stop();
}

void subscribers(vector<string> &subscribers, string arg, char sep=',')
{
  subscribers.clear();
  while (arg.length() > 0) {
    size_t comma = arg.find(sep);
    string subscriber = arg.substr(0,(comma != string::npos) ? comma : arg.length());
    subscribers.push_back(subscriber);
    arg=arg.substr((comma != string::npos) ? comma+1 : arg.length());
  }
}

void configure(const string &config_csv, Commander &commander)
{
  vector < vector < string > > values;
  if (!CSVRead(config_csv,"name,value",values)) {
    cout << "Could not read configuration file '" << config_csv << "'." << endl;
    exit(1);
  }
  map<string,string> cfg;
  for (size_t i=0; i != values.size(); ++i) {
    cfg[values[i][0]]=values[i][1];
  }

  if (cfg.find("commander.publish") != cfg.end()) {
    commander.publish = cfg["commander.publish"];
  }

  if (cfg.find("commander.subscribers") != cfg.end()) {
    subscribers(commander.subscribers,cfg["commander.subscribers"],';');
  }
}

void run(int argc, char **argv) 
{
  Commander commander;

  for (int argi=1; argi<argc; ++argi) {
    if (strcmp(argv[argi],"--configure") == 0) {
      configure(argv[++argi],commander);
      continue;
    }
    if (strcmp(argv[argi],"--publish") == 0) {
      commander.publish = argv[++argi];
      continue;
    }
    if (strcmp(argv[argi],"--subscribers") == 0) {
      subscribers(commander.subscribers,argv[++argi]);
      continue;
    }
  }

  if (argc == 1) {
    configure(CONFIG_CSV,commander);
  }

  commander.start();
  pserver=&commander;
  
  signal(SIGINT, SigIntHandler);

  while (commander.running) {
    string command;
    cout << "?";
    if (!getline(cin,command) || command == "exit") { commander.stop(); }
    else commander.send(command);
  }
  commander.join();
}

int main(int argc, char **argv)
{
  run(argc,argv);
  cout << "done" << endl;
  return 0;
}
