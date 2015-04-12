#include <thread>
#include <string>
#include <vector>
#include <signal.h>
#include "Configure.h"
#include "now.h"

struct Daemon
{
  volatile bool running;
  std::thread *thread;
  volatile float frequency;
  Daemon()  : running(false), thread(0), frequency(1) {}

  void start()
  {
    if (thread == 0) {
      running = true;
      thread = new std::thread(&Daemon::run, this);
    }
  }

  void stop()
  {
    running = false;
    thread->join();
    delete thread;
    thread = 0;
  }

  void run() 
  {
    double t = now();
    while (running) {
      action();
      double dt = now()-t;
      while (running && 1 > dt*frequency) {
	double pause = 1/frequency-dt;
	if (pause > 0.1) {
	  pause = 0.1;
	}
	usleep(1e6*pause);
	dt += pause;
      }
      t += dt;
    }
  }

  virtual void action() {}
  virtual ~Daemon() {}
};

struct Synchronizer : public Daemon
{
  int argc;
  char **argv;
  Configure *cfg;
  bool verbose;

  std::string remote;
  std::string local;
  std::string onchange;
  std::vector<std::string> files;
  std::string cmd;
  
  Synchronizer()
    : argc(0),argv(0),cfg(0),verbose(false)
  {
  }

  ~Synchronizer()
  {
    stop();
    delete cfg;
  }

  void config()
  {
    if (cfg != 0) {
      delete cfg;
      cfg = 0;
    }
    cfg = new Configure();
    cfg->path("../../setup");
    cfg->args("synchronizer.", argv);
    if (argc == 1) cfg->load("config.csv");
    verbose = cfg->flag("synchronizer.verbose", false);
    if (verbose) cfg->show();

    remote = cfg->str("synchronizer.remote");
    local = cfg->str("synchronizer.local");
    onchange = cfg->str("synchronizer.onchange");
    files = cfg->list("synchronizer.files");

    frequency=cfg->num("synchronizer.frequency",frequency);

    cmd = "";
    if (remote != "" && local != "" && files.size() != 0) {
      cmd = "/bin/bash src/synchronizer.sh";
      cmd += " --remote "; cmd += remote;
      cmd += " --local ";  cmd += local;
      for (size_t i=0; i<files.size(); ++i) {
	cmd += " "; cmd += files[i];
      }
    }
  }

  void config(int _argc, char **_argv)
  {
    argc=_argc;
    argv=_argv;
    config();
  }

  void action()
  {
    if (cmd != "") {
      if (system(cmd.c_str()) == 0) {
	config();
	if (onchange != "") {
	  system(onchange.c_str());
	}
      }
    }
  }
};

Synchronizer synchronizer;

void quit(int sig)
{
  synchronizer.running = false;
}

int main(int argc, char** argv)
{
  synchronizer.config(argc,argv);
  synchronizer.start();
  signal(SIGINT, quit);
  signal(SIGQUIT, quit);
  while (synchronizer.running) {
    usleep(100000);
  }
  return 0;
}
