#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <string>
#include <string.h>
#include <csignal>
#include <signal.h>
#include <assert.h>

#include "config.h"
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

  Commander()
  {
    subscribers.push_back(BODY_COMMAND_CONNECT);
    publish = COMMANDER_LISTEN;
  }
};

Commander *pserver=0;

void SigIntHandler(int arg) {
  pserver->stop();
}

void run(int argc, char **argv) 
{
  Commander commander;

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
