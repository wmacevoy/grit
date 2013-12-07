#include <gtkmm.h>
#include <zmq.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <sstream>

#include "Configure.h"
#include "now.h"
#include "SensorsMessage.h"
#include "ZMQHub.h"

Configure cfg;
bool verbose = false;

const int CHARSPERLINE = 64;

SensorsMessage sensors;

template <typename T>
std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

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

class guicmdr : public Gtk::Window
{
protected:
  Glib::RefPtr<Gtk::Builder> builder;
  Gtk::Button *send, *f, *b, *r, *l, *sr, *sl, *h, *hpf, *hpb, *hyl, *hyr;
  Gtk::ColorButton *btn_safe;
	Glib::RefPtr<Gtk::EntryBuffer> cmdBuf;
	Gtk::Entry *ent_cmd;
	Glib::RefPtr<Gtk::TextBuffer> tb_old, tb_resp;
	Gtk::TextView *tv_old, *tv_resp;
	Gdk::Color clr_safe;
	Glib::ustring text;

	int hp, hy;

public:
  guicmdr(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) : Gtk::Window(cobject), builder(refGlade) {
		builder->get_widget("send", send);
		builder->get_widget("btn_f", f);
		builder->get_widget("btn_b", b);
		builder->get_widget("btn_r", r);
		builder->get_widget("btn_l", l);
		builder->get_widget("btn_sr", sr);
		builder->get_widget("btn_sl", sl);
		builder->get_widget("btn_h", h);
		builder->get_widget("btn_hp_f", hpf);
		builder->get_widget("btn_hp_b", hpb);
		builder->get_widget("btn_hy_l", hyl);
		builder->get_widget("btn_hy_r", hyr);
		builder->get_widget("command", ent_cmd);
		builder->get_widget("oldCommands", tv_old);
		builder->get_widget("response", tv_resp);
		builder->get_widget("btn_safe", btn_safe);

		tb_old = Gtk::TextBuffer::create();
		tb_resp = Gtk::TextBuffer::create();

		send->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_send_clicked) );
		f->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_f_clicked) );
		b->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_b_clicked) );
		r->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_r_clicked) );
		l->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_l_clicked) );
		sr->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_sr_clicked) );
		sl->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_sl_clicked) );
		h->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_h_clicked) );
		hpf->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_hpf_clicked) );
		hpb->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_hpb_clicked) );
		hyl->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_hyl_clicked) );
		hyr->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_hyr_clicked) );

		Glib::signal_timeout().connect( sigc::mem_fun(*this, &guicmdr::on_timer), 100 );

		hp = 0;
		hy = 0;

	}

	~guicmdr(){}

	bool on_timer() {
		updateSafety();
		return true;
	}

	void updateSafety() {
		int r=255-sensors.s[0],g=255-sensors.s[1],b=255-sensors.s[2];
		clr_safe.set_rgb(255*r,255*g,255*b);
	}

	void on_button_hpf_clicked() {
		if(verbose) std::cout << "btn_hpf clicked" << std::endl;
		hp--;
		ent_cmd->set_text("hp " + NumberToString(hp));
	}

	void on_button_hpb_clicked() {
		if(verbose) std::cout << "btn_hpb clicked" << std::endl;
		hp++;
		ent_cmd->set_text("hp " + NumberToString(hp));
	}

	void on_button_hyl_clicked() {
		if(verbose) std::cout << "btn_hyl clicked" << std::endl;
		hy--;
		ent_cmd->set_text("hy " + NumberToString(hy));
	}

	void on_button_hyr_clicked() {
		if(verbose) std::cout << "btn_hyr clicked" << std::endl;
		hy++;
		ent_cmd->set_text("hy " + NumberToString(hy));
	}

	void on_button_send_clicked() {
		if(verbose) std::cout << "btn_send clicked" << std::endl;
		text = ent_cmd->get_text();
		tb_old->insert_at_cursor(" " + text);
		tv_old->set_buffer(tb_old);
	}

	void on_button_f_clicked() {
		if(verbose) std::cout << "btn_f clicked" << std::endl;
		ent_cmd->set_text("bf");
	}

	void on_button_b_clicked() {
		if(verbose) std::cout << "btn_b clicked" << std::endl;
		ent_cmd->set_text("bb");
	}

	void on_button_r_clicked() {
		if(verbose) std::cout << "btn_r clicked" << std::endl;
		ent_cmd->set_text("br");
	}

	void on_button_l_clicked() {
		if(verbose) std::cout << "btn_l clicked" << std::endl;
		ent_cmd->set_text("bl");
	}

	void on_button_sr_clicked() {
		if(verbose) std::cout << "btn_sr clicked" << std::endl;
		ent_cmd->set_text("bsr");
	}

	void on_button_sl_clicked() {
		if(verbose) std::cout << "btn_sl clicked" << std::endl;
		ent_cmd->set_text("bsl");
	}

	void on_button_h_clicked() {
		if(verbose) std::cout << "btn_h clicked" << std::endl;
		ent_cmd->set_text("bh");
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

	sensorsRx = std::shared_ptr < SensorsRx > (new SensorsRx());

  Gtk::Main kit(argc,argv);
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("src/MojUI.glade");

  builder->get_widget_derived("main_window", gui);
  kit.run(*gui);
  gui->end();

	return 0;
}
