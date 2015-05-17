#include <csignal>
#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <iomanip>

#include "GPIO.h"
#include "AnalogIn.h"
#include "Tone.h"
#include "now.h"
#include "Configure.h"
#include "Foot.h"

std::string grit_id()
{
  std::string ans;

  char *HOME=getenv("HOME");
  std::string id_path=HOME;
  id_path += "/grit/id";
  std::ifstream id_file(id_path.c_str());
  std::getline(id_file,ans);
  return ans;
}

const std::string id(grit_id());

Configure cfg;
bool verbose = false;
volatile bool running = true;

std::vector < std::shared_ptr < Foot > > feet;

void stop(int arg) {
  running = false;
}

int main(int argc, char *argv[])
{
  signal(SIGINT, stop);
  signal(SIGTERM, stop);
  signal(SIGQUIT, stop);

  system("sudo modprobe adc");

  std::string id=grit_id();
  cfg.path("../../../setup");
  cfg.args(id + ".",argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag(id + ".verbose",false);
  if (verbose) cfg.show();

  int devs=cfg.num(id + ".devs");
  for (int dev=0; dev<devs; ++dev) {
    feet.push_back(Foot::factory(id,cfg,dev));
  }

  for (auto foot : feet) foot->start();

  while (running) {
    usleep(100000);
  }

  for (auto foot : feet) foot->stop();

  return 0;
}
