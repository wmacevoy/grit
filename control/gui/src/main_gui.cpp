#include <gtkmm.h>
#include <zmq.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>

#include "Configure.h"
#include "now.h"

Configure cfg;
bool verbose = false;
volatile bool die = false;

const int NUM_SERVOS = 68;
const int NUM_SENSORS = 14;

using namespace std;

template <typename T>
string NumberToString ( T Number )
{
  ostringstream ss;
  ss << Number;
  return ss.str();
}

void quitproc(int param) {
	die = true;
}

class RobotWatcher
{
private:
	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::Window *w;
	Gtk::Main kit;
	Gtk::ColorButton *temp_button[200];
	Gtk::ColorButton *pressure_button[4];
	Gdk::Color sev_colors[4];
	void *temp_context;
	void *sensor_context;
	void *temp_sub;
	void *sensor_sub;
        string address_t;
        string address_s;
	int hwm, linger, rcc;
	float t1, t2, timeOut;
 	int SLEEP_TIME;
	int32_t sensors[NUM_SENSORS];
	int32_t temps[NUM_SERVOS];
	
public:
	RobotWatcher(int argc, char *argv[])
		: kit(argc, argv)
	{
		hwm = 1;
		linger = 25;
		t1=0;
		t2=0;
		timeOut=0.5;

		signal(SIGINT, quitproc);
		signal(SIGTERM, quitproc);
		signal(SIGQUIT, quitproc);

		cfg.path("../../setup");
		cfg.args("gui.requester.", argv);
		if (argc == 1) cfg.load("config.csv");
		verbose = cfg.flag("gui.requester.verbose", false);
		if (verbose) cfg.show();
		
		address_t = cfg.str("gui.requester.address_t").c_str();
		address_s = cfg.str("gui.requester.address_s").c_str();
		SLEEP_TIME = (int)(cfg.num("gui.requester.sleep_t"));

		builder = Gtk::Builder::create();
		
		temp_context = zmq_ctx_new ();
		temp_sub = zmq_socket(temp_context, ZMQ_SUB);
		zmq_setsockopt(temp_sub, ZMQ_SUBSCRIBE, "", 0);
		zmq_setsockopt(temp_sub,ZMQ_RCVHWM,&hwm,sizeof(hwm));
		zmq_setsockopt(temp_sub,ZMQ_LINGER,&linger,sizeof(linger));

		sensor_context = zmq_ctx_new ();
		sensor_sub = zmq_socket(temp_context, ZMQ_SUB);
		zmq_setsockopt(sensor_sub, ZMQ_SUBSCRIBE, "", 0);
		zmq_setsockopt(sensor_sub,ZMQ_RCVHWM,&hwm,sizeof(hwm));
		zmq_setsockopt(sensor_sub,ZMQ_LINGER,&linger,sizeof(linger));
		
		if (zmq_connect(temp_sub, address_t.c_str()) != 0)
		{
		  cout << "Error initializing 0mq, temp requester: " << address_t << endl;
		}
		if (zmq_connect(sensor_sub, address_t.c_str()) != 0)
		{
		  cout << "Error initializing 0mq, temp requester: " << address_s << endl;
		}

		sev_colors[0].set_rgb(255,0,0);
		sev_colors[1].set_rgb(0,255,0);
		sev_colors[2].set_rgb(125,125,0);
		sev_colors[3].set_rgb(0,0,0);

		for (int i = 0; i < NUM_SERVOS; i++)
		  temps[i]=0;

		for (int i = 0; i < NUM_SENSORS; i++)
		  sensors[i] = 0;
	}

	~RobotWatcher(){}

	bool subscribe_temperatures(int32_t temps[])
	{
	  bool ret = false;
		zmq_msg_t msg;
		zmq_msg_init (&msg);
		
		if(zmq_recvmsg(temp_sub, &msg, ZMQ_DONTWAIT) > 0) {
			memcpy(temps, zmq_msg_data(&msg), zmq_msg_size(&msg));			
			ret = true;
			t1=now();
		}
		zmq_msg_close(&msg);	

		t2 = now();
		if(t2 - t1 > timeOut) {
			zmq_close(temp_sub);
			temp_sub = zmq_socket(temp_context, ZMQ_SUB);
			if(zmq_setsockopt(temp_sub, ZMQ_SUBSCRIBE, "", 0) == 0) {
				if(zmq_setsockopt(temp_sub, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(temp_sub, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
						if(zmq_setsockopt(temp_sub, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
							rcc = zmq_connect(temp_sub, address_t.c_str());
						}
					}
				}
			}
		}
		return ret;
	}

	bool subscribe_sensors(int32_t sensors[])
	{
	  bool ret = false;
		zmq_msg_t msg;
		zmq_msg_init (&msg);
		
		if(zmq_recvmsg(sensor_sub, &msg, ZMQ_DONTWAIT) > 0) {
			memcpy(sensors, zmq_msg_data(&msg), zmq_msg_size(&msg));		
			ret = true;
			t1=now();
		}
		zmq_msg_close(&msg);	

		t2 = now();
		if(t2 - t1 > timeOut) {
			zmq_close(sensor_sub);
			sensor_sub = zmq_socket(sensor_context, ZMQ_SUB);
			if(zmq_setsockopt(sensor_sub, ZMQ_SUBSCRIBE, "", 0) == 0) {
				if(zmq_setsockopt(sensor_sub, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(sensor_sub, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
						if(zmq_setsockopt(sensor_sub, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
							rcc = zmq_connect(sensor_sub, address_s.c_str());
						}
					}
				}
			}
		}
		return ret;
	}

	void update_colors_temps(int32_t temps[], int size)
	{
		int sev = 3;
		for (int i = 0; i < size; i+=2)
		{
			if (temps[i+1] > 65)
				sev = 0;
			else if (temps[i+1] > 45)
				sev = 1;
			else if (temps[i+1] > 0)
				sev = 2;
			else
				sev = 3;

			temp_button[temps[i]]->set_color(sev_colors[sev]);
		}
	}

	void update_colors_pressure(int32_t sensors[], int size) 
	{
		int sev = 3;
		for(int i = 10; i < size; i++)
		{
			if (sensors[i] > 900)
				sev = 0;
			else if (sensors[i] > 750)
				sev = 1;
			else if (sensors[i] >= 0)
				sev = 2;
			else
				sev = 3;

			pressure_button[i]->set_color(sev_colors[sev]);
		}
	}

	void init()
	{
		builder->add_from_file("main.xml");
		builder->get_widget("winStatus",w);
		for (int i =11; i < 14; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =21; i < 24; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =31; i < 34; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =41; i < 44; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =51; i < 60; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =61; i < 70; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =91; i < 92; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =93; i < 95; i++)
		{
			string btn_string = "sig" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),temp_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}
		for (int i =1; i < 5; i++)
		{
			string btn_string = "sigL" + NumberToString(i);
			builder->get_widget(btn_string.c_str(),pressure_button[i]);
			//temp_button[i]->set_sensitive(false);			
		}

		w->signal_delete_event().connect(sigc::mem_fun(this, &RobotWatcher::end1));
		Glib::signal_timeout().connect(sigc::mem_fun(this, &RobotWatcher::on_timer),
          SLEEP_TIME );

	}

	bool on_timer()
	{
		bool got = subscribe_temperatures(temps);
		if (got)
		  update_colors_temps(temps, NUM_SERVOS);
	
		got = subscribe_sensors(sensors);
		if (got)
		  update_colors_pressure(sensors, NUM_SENSORS);
		
		return true;
	}

	bool end1(GdkEventAny *)
	{
		Gtk::Main::quit(); 
		die = true;   
		return true;
	}

	void run()
	{
	  kit.run(*w);
	  end();
	}

	void end()
	{
		std::cout << std::endl << "Quitting..." << std::endl;
		delete w;
		zmq_close(temp_sub);
		zmq_close(sensor_sub);
		zmq_ctx_destroy(temp_context);
		zmq_ctx_destroy(sensor_context);
		std::cout << "--done!" << std::endl;
	}
};


int main(int argc, char *argv[])
{
	RobotWatcher gui(argc,argv);
	gui.init();
	gui.run();
	return 0;
}
