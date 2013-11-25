#include <gtkmm.h>
#include <zmq.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <mutex>

#include "Configure.h"
#include "now.h"

Configure cfg;
bool verbose = false;
bool die = false;

const int NUM_SERVOS = 68;

std::mutex lockBuffer;

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
	void *temp_sub;
        string address_t;
        string address_s;
	int hwm, linger, rcc;
	float t1, t2, timeOut;
 	int SLEEP_TIME;
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
		SLEEP_TIME = (int)(cfg.num("gui.requester.sleep_t"));

		builder = Gtk::Builder::create();
		
		temp_context = zmq_ctx_new ();
		temp_sub = zmq_socket(temp_context, ZMQ_SUB);
		zmq_setsockopt(temp_sub, ZMQ_SUBSCRIBE, "", 0);
		zmq_setsockopt(temp_sub,ZMQ_RCVHWM,&hwm,sizeof(hwm));
		zmq_setsockopt(temp_sub,ZMQ_LINGER,&linger,sizeof(linger));
		
		if (zmq_connect(temp_sub, address_t.c_str()) != 0)
		{
		  cout << "Error initializing 0mq, temp requester: " << address_t << endl;
		}
		sev_colors[0].set_rgb(255,0,0);
		sev_colors[1].set_rgb(0,255,0);
		sev_colors[2].set_rgb(125,125,0);
		sev_colors[3].set_rgb(0,0,0);		
	}
	~RobotWatcher(){}

	bool request_temperatures(int32_t temps[])
	{
	  bool ret = false;
		zmq_msg_t msg;
		zmq_msg_init (&msg);
		
		if(zmq_recvmsg(temp_sub, &msg, ZMQ_DONTWAIT) > 0) {
			lockBuffer.lock();
			memcpy(temps, zmq_msg_data(&msg), zmq_msg_size(&msg));
			lockBuffer.unlock();			
			ret = true;
			t1=now();
		}
		zmq_msg_close(&msg);	

		t2 = now();
		if(t2 - t1 > timeOut) {
			zmq_close(temp_sub);
			temp_sub = zmq_socket(temp_context, ZMQ_REQ);
		
			if(zmq_setsockopt(temp_sub, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(temp_sub, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == 0) {
					if(zmq_setsockopt(temp_sub, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
						rcc = zmq_connect(temp_sub, address_t.c_str());
					}
				}
			}
		}
		return ret;
	}

	void update_colors(int temps[], int size)
	{
		lockBuffer.lock();
		for (int i = 0; i < size - 2; i+=2)
		{
			int sev = 3;
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
		lockBuffer.unlock();
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
	}

       thread *update_thread;
       void updategui()
       {
		int32_t temps[NUM_SERVOS];
		for (int i = 0; i < NUM_SERVOS; i++)
		  temps[i]=0;
		while (!die){
			bool got = request_temperatures(temps);
			if (got)
			  update_colors(temps, NUM_SERVOS);
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
		}
		end();
         }

	void run()
	{
	  update_thread = NULL;
	  update_thread = new thread(&RobotWatcher::updategui,this);
	  kit.run(*w);
	  update_thread->join();
	  delete update_thread;
	}

	void end()
	{
		delete w;
		zmq_close(temp_sub);
		zmq_ctx_destroy(temp_context);
	}
};


int main(int argc, char *argv[])
{
	RobotWatcher gui(argc,argv);
	gui.init();
	gui.run();
	return 0;
}
