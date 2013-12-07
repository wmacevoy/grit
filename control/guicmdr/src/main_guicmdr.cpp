#include <gtkmm.h>
#include <zmq.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <map>

#include "Configure.h"

Configure cfg;
bool verbose = false;

class guicmdr : public Gtk::Window
{
protected:
  Glib::RefPtr<Gtk::Builder> builder;
  Gtk::Button* send;

public:
  guicmdr(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) : Gtk::Window(cobject), builder(refGlade) {
		builder->get_widget("send", send);
	}

	~guicmdr(){}

	//Signal handler
  void end2()
  {
    Gtk::Main::quit();
  }

	void end()
  {
    std::cout << std::endl << "--done!" << std::endl;
  }

};

guicmdr* gui;

void quitproc(int param) {
  gui->end2();
}

int main(int argc, char** argv) {
	cfg.path("../../setup");
  cfg.args("guicmdr.", argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("guicmdr.verbose", false);
  if (verbose) cfg.show();

  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);

  Gtk::Main kit(argc,argv);
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("src/MojUI.glade");

  builder->get_widget_derived("main_window", gui);
  kit.run(*gui);
  gui->end();

	return 0;
}
