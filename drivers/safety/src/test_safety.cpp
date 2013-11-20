#include "CreateSafetyClient.h"
#include "CreateSafetyServer.h"
#include "now.h"

#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

class Proxy
{
public:
  bool running;
  std::thread *go;
  SafetySP safety;
  bool safe;

  Proxy(int port)
  {
    ostringstream me;
    me << "tcp://*:" << port;
    safety = CreateSafetyClient(me.str(),"tcp://127.0.0.1:6000",1);
    running = true;
    safe=false;
    safety->safe(false);
    go = new std::thread(&Proxy::run,this);
  }

  ~Proxy()
  {
    running = false;
    go->join();
    delete go;
    go = 0;
  }

  void run()
  {
    float change = now() + rand()%3 + 5;
    while (running) {
      sleep(1);
      if (change < now()) {
	safe = !safe;
	safety->safe(safe);
	change = now() + rand()%3 + 5;
      }
    }
  }
};

int main()
{
  vector < string > subscribers;
  subscribers.push_back("tcp://127.0.0.1:6001");
  subscribers.push_back("tcp://127.0.0.1:6002");

  SafetySP server = CreateSafetyServer("tcp://*:6000",subscribers,4);
  Proxy body(6001);
  Proxy servos(6002);

  double t0=now();
  
  for (;;) {
    sleep(1);

    double t=now()-t0;
    if (t > 20.0) break;

    cout << t 
	 << "," << body.safe << "," << body.safety->safe() << "," << body.safety->warn()
	 << "," << servos.safe << "," << servos.safety->safe() << "," << servos.safety->warn()
	 << endl;
  }

  return 0;
}
