#include <iostream>
#include <signal.h>

#include "now.h"
#include "Configure.h"
#include "CreateSafetyServer.h"

using namespace std;

Configure cfg;
bool verbose;
SafetySP server;

void quit(int sig)
{
  server.reset();
}

void config(int argc, char** argv)
{
  cfg.path("../../setup");
  cfg.args("safety.", argv);
  if (argc == 1) cfg.load("config.csv");
  verbose = cfg.flag("safety.verbose", false);
  if (verbose) cfg.show();
}

int main(int argc, char** argv)
{
  config(argc,argv);

  signal(SIGINT, quit);
  signal(SIGTERM, quit);
  signal(SIGQUIT, quit);

  server = CreateSafetyServer(cfg.str("safety.publish"),cfg.list("safety.subscribers"),cfg.num("safety.rate"),cfg.num("safety.delayoff"));

  while (server) {
    sleep(1);
    if (verbose) {
      double t=now();
      bool safe=server->safe();
      bool warn=server->warn();
      cout << t << "," << safe << "," << warn << endl;
    }
  }
  return 0;
}
