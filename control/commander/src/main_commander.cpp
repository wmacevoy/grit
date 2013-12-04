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

#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"
#include "Lock.h"
#include "Configure.h"

using namespace std;

Configure cfg;

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

  bool rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    char *data = (char*) msg.data();
    size_t size = *(uint16_t*)data;
    string reply(data+2,size);
    cout << "\rreply: " << reply << endl << "?";
    cout.flush();
    return true;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    Lock lock(sendsMutex);
    bool ok=true;

    while (!sends.empty()) {
      string &message = *sends.begin();
      uint16_t size = (message.size() < BODY_MESSAGE_MAXLEN) ? message.size() : BODY_MESSAGE_MAXLEN;
      ZMQMessage msg(size+2);
      char *data = (char*)msg.data();
      *(uint16_t*)data = size;
      memcpy(data+2,&message[0],size);
      if (msg.send(socket) == 0) ok=false;
      sends.pop_front();
    }
    return ok;
  }
};

Commander *pserver=0;

void SigIntHandler(int arg) {
  pserver->stop();
}

void run() 
{
  Commander commander;
  commander.publish = cfg.str("commander.publish");
  commander.subscribers = cfg.list("commander.subscribers");
  commander.rxTimeout = 1e6;
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
  cfg.path("../../setup");
  cfg.args("commander.",argv);
  if (argc == 1) cfg.load("config.csv");
  if (cfg.flag("commander.verbose",false)) cfg.show();
  run();
  cout << "done" << endl;
  return 0;
}
