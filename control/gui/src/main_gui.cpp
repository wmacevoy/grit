#include <gtkmm.h>
#include <zmq.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <map>

#include "CreateZMQServoListener.h"
#include "Configure.h"
#include "now.h"
#include "SensorsMessage.h"
#include "ZMQHub.h"

Configure cfg;
bool verbose = false;

const double colorMapper = USHRT_MAX / 255;

SPServoController servoController;
std::map<int,Servo*> servos;
SensorsMessage sensors;

class SensorsRx : public ZMQHub
{
public:
  SensorsRx() {
    subscribers.push_back(cfg.str("proxysensors.subscribe"));
    start();
  }

  bool rx(ZMQSubscribeSocket &socket) {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    memcpy(&sensors,msg.data(),sizeof(SensorsMessage));
    if (verbose) {
      std::cout << "a=[" << sensors.a[0] << "," << sensors.a[1] << "," << sensors.a[2] << "]" << std::endl;
    }
    return true;
  }
  bool tx(ZMQPublishSocket &socket) { return true; }
};

std::shared_ptr < SensorsRx > sensorsRx;

using namespace std;

template <typename T>
string NumberToString ( T Number )
{
  ostringstream ss;
  ss << Number;
  return ss.str();
}

class RobotWatcher : public Gtk::Window
{
protected:
  Glib::RefPtr<Gtk::Builder> builder;
  std::map<int, Gtk::ColorButton*> buttons;
  std::map<int, Gtk::ColorButton*>::iterator im;
  Gdk::Color sev_colors[5];
  Gtk::Label *lblTop, *lblTemp, *lblAccel, *lblGyro, *lblCompass, *lblSafety;
  int SLEEP_TIME;
	
public:
  RobotWatcher(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) : Gtk::Window(cobject), builder(refGlade)
  {
    SLEEP_TIME = (int)(cfg.num("gui.sleep_t"));
		
    sev_colors[0].set_rgb(USHRT_MAX,0,0);
    sev_colors[2].set_rgb(USHRT_MAX,USHRT_MAX,0);
    sev_colors[2].set_rgb(0,USHRT_MAX,0);
    sev_colors[3].set_rgb(0,0,0);
    sev_colors[4].set_rgb(0,0,0); //Used for safety light only
	  
    for (int i =11; i < 14; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }

    for (int i =21; i < 24; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }
    for (int i =31; i < 34; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }

	  
    for (int i =41; i < 44; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }

    for (int i =51; i < 60; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }
    for (int i =61; i < 70; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }
    for (int i =91; i < 92; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }
    for (int i =93; i < 95; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);
	//temp_button[i]->set_sensitive(false);			
      }
    for (int i =101; i <= 114; i++)
      {
        string btn_string = "sen" + NumberToString(i);
        builder->get_widget(btn_string.c_str(),buttons[i]);
      }
    builder->get_widget("lblTop", lblTop);
    builder->get_widget("lblTemp", lblTemp);

    builder->get_widget("lblAccel", lblAccel);
    builder->get_widget("lblGyro", lblGyro);
    builder->get_widget("lblCompass", lblCompass);
    builder->get_widget("lblSafety", lblSafety);
	
    lblTop->set_text("Top: N/A");

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &RobotWatcher::on_timer), SLEEP_TIME );

  }

  ~RobotWatcher(){}

  void update_colors_temps(int32_t temps[], int size)
  {
    int sev = 3, max = 0;
    for (int i = 0; i < size; i+=2)
      {
	if(temps[i+1] > max) max = temps[i+1];
	if (temps[i+1] > 60)
	  sev = 0;
	else if (temps[i+1] > 45)
	  sev = 1;
	else if (temps[i+1] > 0)
	  sev = 2;
	else
	  sev = 3;

	im = buttons.find(temps[i]);
	if(im != buttons.end())
	  im->second->set_color(sev_colors[sev]);			
	lblTop->set_text("Top: " + NumberToString(temps[i+1]));
      }
  }

  void update_servos()
  {
    std::vector <int32_t > temps(2*servos.size(),0);
    int count=0;
    for (std::map<int,Servo*>::iterator i=servos.begin(); i != servos.end(); ++i) {
      temps[count++]=i->first;
      temps[count++]=i->second->temp();
    }
    update_colors_temps(&temps[0],2*servos.size());
  }


  void update_colors_pressure(int32_t sensors[]) 
  {
    int sev = 3;
    for(int i = 1; i <= 9; i++)
      {
        im = buttons.find(i + 100);
	if(im != buttons.end())
	  im->second->set_color(sev_colors[sev]);
      }

    //Update safety
    im = buttons.find(110);
	if(im != buttons.end()) {
	  sev_colors[4].set_rgb(sensors[12] * colorMapper, sensors[13] * colorMapper, sensors[14] * colorMapper);
          im->second->set_color(sev_colors[4]);
        }

    //Update leg pressures
    for(int i = 11; i <= 14; i++)
      {
	if (sensors[i] > 900)
	  sev = 0;
	else if (sensors[i] > 750)
	  sev = 1;
	else if (sensors[i] >= 0)
	  sev = 2;
	else
	  sev = 3;
	
	
	im = buttons.find(i + 100);
	if(im != buttons.end())
	  im->second->set_color(sev_colors[sev]);
      }
  }

  void update_sensors()
  {
    std::vector < int32_t > data;

    //Sen 101-103, index 0-2
    data.push_back(sensors.a[0]);
    data.push_back(sensors.a[1]);
    data.push_back(sensors.a[2]);

    //Sen 104-106, index 3-5
    data.push_back(sensors.c[0]);
    data.push_back(sensors.c[1]);
    data.push_back(sensors.c[2]);

    //Sen 107-109, index 6-8
    data.push_back(sensors.g[0]);
    data.push_back(sensors.g[1]);
    data.push_back(sensors.g[2]);

    //Sen 111-114, index 9-12
    data.push_back(sensors.p[0]);
    data.push_back(sensors.p[1]);
    data.push_back(sensors.p[2]);
    data.push_back(sensors.p[3]);

    //Sen 110, index 12-14
    data.push_back(sensors.s[0]);
    data.push_back(sensors.s[1]);
    data.push_back(sensors.s[2]);
    update_colors_pressure(&data[0]);
  }
  
  bool on_timer()
  {
    update_servos();
    update_sensors();
    return true;
  }

  //Signal handler
  void end2()
  {
    Gtk::Main::quit();
  }
  
  void end()
  {
    std::cout << "done." << std::endl;
  }
};

RobotWatcher* gui;

void quitproc(int param) {
  gui->end2();
}

int main(int argc, char *argv[])
{
  cfg.path("../../setup");
  cfg.args("gui.", argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("gui.verbose", false);
  if (verbose) cfg.show();

  servoController = SPServoController(CreateZMQServoListener(cfg.str("proxyservos.subscribe")));
  std::set<std::string> names = cfg.servoNames();
  for (std::set<std::string>::iterator i=names.begin(); i!=names.end(); ++i) {
    int id=atoi(cfg.servo(*i,"id").c_str());
    servos[id]=servoController->servo(id);
  }
  servoController->start();

  sensorsRx = std::shared_ptr < SensorsRx > (new SensorsRx());
	
  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);

  Gtk::Main kit(argc,argv);
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("src/main.xml");

  builder->get_widget_derived("winStatus", gui);
  kit.run(*gui);
  gui->end();

  return 0;
}
