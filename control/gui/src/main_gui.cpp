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

enum Color {RED, YELLOW, GREEN, BLACK, SAFETY, BLUE, ORANGE, GREY };

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
  Gdk::Color sev_colors[8];
  Gtk::Label *lblTop, *lblSafety;
  Gtk::Label *lblA0, *lblA1, *lblA2, *lblG0, *lblG1, *lblG2, *lblC0, *lblC1, *lblC2;
  int SLEEP_TIME;
	
public:
  RobotWatcher(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) : Gtk::Window(cobject), builder(refGlade)
  {
    SLEEP_TIME = (int)(cfg.num("gui.sleep_t"));
		
    sev_colors[RED].set_rgb(USHRT_MAX,0,0);
    sev_colors[YELLOW].set_rgb(USHRT_MAX,USHRT_MAX,0);
    sev_colors[GREEN].set_rgb(0,USHRT_MAX,0);
    sev_colors[BLACK].set_rgb(0,0,0);
    sev_colors[SAFETY].set_rgb(0,0,0); //Used for safety light only
    sev_colors[BLUE].set_rgb(0,USHRT_MAX/2+USHRT_MAX/5,USHRT_MAX); //BLUE
    sev_colors[ORANGE].set_rgb(USHRT_MAX,USHRT_MAX/2+USHRT_MAX/7,0); //ORANGE
    sev_colors[GREY].set_rgb(USHRT_MAX/2, USHRT_MAX/2,USHRT_MAX/2); //GREY
	  
    for (int i =11; i < 14; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);		
      }

    for (int i =21; i < 24; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);			
      }
    for (int i =31; i < 34; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);			
      }
	  
    for (int i =41; i < 44; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);		
      }

    for (int i =51; i < 60; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);		
      }
    for (int i =61; i < 70; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);			
      }
    for (int i =91; i < 92; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);			
      }
    for (int i =93; i < 95; i++)
      {
	string btn_string = "sig" + NumberToString(i);
	builder->get_widget(btn_string.c_str(),buttons[i]);		
      }
    for (int i =101; i <= 105; i++)
      {
        string btn_string = "sen" + NumberToString(i);
        builder->get_widget(btn_string.c_str(),buttons[i]);
      }

    builder->get_widget("lblTop", lblTop);
    builder->get_widget("lblSafety", lblSafety);
    builder->get_widget("lblA0", lblA0);
    builder->get_widget("lblA1", lblA1);
    builder->get_widget("lblA2", lblA2);
    builder->get_widget("lblG0", lblG0);
    builder->get_widget("lblG1", lblG1);
    builder->get_widget("lblG2", lblG2);
    builder->get_widget("lblC0", lblC0);
    builder->get_widget("lblC1", lblC1);
    builder->get_widget("lblC2", lblC2);
	
	
    lblTop->set_text("Top: N/A");

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &RobotWatcher::on_timer), SLEEP_TIME );
  }

  ~RobotWatcher(){}

  void update_colors_temps(int32_t temps[], int size)
  {
    int sev = BLACK, max = 0, max_servo = 0;
    for (int i = 0; i < size; i+=2)
      {
	if(temps[i+1] > max){ max = temps[i+1]; max_servo=temps[i];}
	//Servos start shaking at 75
	if (temps[i+1] >= 68) //Point at which lights turn red
	  sev = RED;
	else if (temps[i+1] > 60) //Orange
	  sev = ORANGE;
	else if (temps[i+1] > 50) //Point at which lights turn yellow
	  sev = YELLOW;
	else if (temps[i+1] > 35) //Green
	  sev = GREEN;
	else if (temps[i+1] == 1) // GREY STALE
	  sev = GREY;
	else if (temps[i+1] > 0) //BLUE
	  sev = BLUE;
	else
	  sev = BLACK;

	im = buttons.find(temps[i]);
	if(im != buttons.end()) {
	  im->second->set_color(sev_colors[sev]);	
	}		
	lblTop->set_text("Top: ID>" + NumberToString(max_servo) + "  Temp> " + NumberToString(max));
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

  void update_sensors()
  {
    int sev = GREY;

    //Update accel, gyro, compass 
    lblA0->set_text("A[0]: " + NumberToString(sensors.a[0]));
    lblA1->set_text("A[1]: " + NumberToString(sensors.a[1]));
    lblA2->set_text("A[2]: " + NumberToString(sensors.a[2]));

    lblG0->set_text("G[0]: " + NumberToString(sensors.g[0]));
    lblG1->set_text("G[1]: " + NumberToString(sensors.g[1]));
    lblG2->set_text("G[2]: " + NumberToString(sensors.g[2]));

    lblC0->set_text("C[0]: " + NumberToString(sensors.c[0]));
    lblC1->set_text("C[1]: " + NumberToString(sensors.c[1]));
    lblC2->set_text("C[2]: " + NumberToString(sensors.c[2]));

    //Update safety
    im = buttons.find(105);
    if(im != buttons.end()) {
      int r=255-sensors.s[0],g=255-sensors.s[1],b=255-sensors.s[2];
      sev_colors[SAFETY].set_rgb(255*r,255*g,255*b); 
      im->second->set_color(sev_colors[SAFETY]);
    }
    
    //Update leg pressures
    for(int i = 0; i < 4; i++)
      {
	if (sensors.p[i] > 900)
	  sev = GREEN;
	else if (sensors.p[i] > 750)
	  sev = YELLOW;
	else if (sensors.p[i] >= 0)
	  sev = RED;
	else
	  sev = BLACK;
	
	im = buttons.find(i + 101);
	if(im != buttons.end()) {
	  im->second->set_color(sev_colors[sev]);
	}
      }
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
    std::cout << std::endl << "--done!" << std::endl;
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
