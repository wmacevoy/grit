#include <iostream>
#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <string>
#include <string.h>
#include <csignal>
#include <signal.h>
#include <assert.h>

#include "BodyMessage.h"
#include "CSVRead.h"
#include "ZMQHub.h"
#include "Lock.h"
#include "Configure.h"
#include "CreateSafetyClient.h"

#include <gtkmm.h>
#include <zmq.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <unistd.h>
#include <sys/wait.h>

#include "Configure.h"
#include "now.h"

Configure cfg;
bool verbose = false;
SafetySP safety;

class Commander : public ZMQHub
{
public:
  std::mutex sendsMutex;
  std::mutex recvsMutex;
  std::list < std::string > sends;
  std::list < std::string > recvs;
  
  void send(const std::string &content)
  {
    Lock lock(sendsMutex);
    sends.push_back(content);
  }

  bool recv(std::string &content)
  {
    Lock lock(recvsMutex);
    if (!recvs.empty()) {
      content=recvs.front();
      recvs.pop_front();
      return true;
    } else {
      return false;
    }
  }

  bool rx(ZMQSubscribeSocket &socket)
  {
    ZMQMessage msg;
    if (msg.recv(socket) == 0) return false;
    char *data = (char*) msg.data();
    size_t size = *(uint16_t*)data;
    std::string reply(data+2,size);
    { 
      Lock lock(recvsMutex);
      recvs.push_back(reply);
    }
    return true;
  }

  bool tx(ZMQPublishSocket &socket)
  {
    Lock lock(sendsMutex);
    bool ok=true;

    while (!sends.empty()) {
      std::string &message = *sends.begin();
      uint16_t size = (message.size() < BODY_MESSAGE_MAXLEN) ? message.size() : BODY_MESSAGE_MAXLEN;
      ZMQMessage msg(size+2);
      char *data = (char*)msg.data();
      *(uint16_t*)data = size;
      memcpy(data+2,&message[0],size);
      if (msg.send(socket) == 0) ok=false;
      sends.pop_front();
    }
    return ok;
  }
};

std::shared_ptr < Commander > commander;


template <typename T>
std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

class guicmdr : public Gtk::Window
{
protected:
  Glib::RefPtr<Gtk::Builder> builder;
  Gtk::Button *send, *f, *b, *r, *l, *sr, *sl, *h, *hpf, *hpb, *hyl, *hyr, *safe_on, *safe_off;
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
		builder->get_widget("btn_safe_on", safe_on);
		builder->get_widget("btn_safe_off", safe_off);

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
		safe_on->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_safe_on_clicked) );
		safe_off->signal_clicked().connect( sigc::mem_fun(*this, &guicmdr::on_button_safe_off_clicked) );

		Glib::signal_timeout().connect( sigc::mem_fun(*this, &guicmdr::on_timer), 100 );

		//signal_key_press_event().connect(sigc::mem_fun(*this,&guicmdr::on_window_key_press_event), false);

		hp = 0;
		hy = 0;
	}

  bool on_timer()
  {
    std::string reply;
    while (commander->recv(reply)) {
      tb_resp->insert_at_cursor((reply+"\n").c_str());
      tv_resp->set_buffer(tb_resp);
    }
		return true;
  }

	~guicmdr() {}


	void on_button_send_clicked() {
		if(verbose) std::cout << "btn_send clicked" << std::endl;
		text = ent_cmd->get_text();
		if(text != "") {
		  commander->send(text);
		  tb_old->insert_at_cursor(" " + text);
		  tv_old->set_buffer(tb_old);
		}
	}

	void on_button_safe_on_clicked() {
		if(verbose) std::cout << "safe_on clicked" << std::endl;
		safety->safe(true);
		//		ent_cmd->set_text("safe on");
	}

	void on_button_safe_off_clicked() {
		if(verbose) std::cout << "safe_on clicked" << std::endl;
		safety->safe(false);
		//		ent_cmd->set_text("safe off");
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
  commander.reset();
}

int main(int argc, char** argv) {
	cfg.path("../../setup");
  cfg.args("guicmdr.", argv);
  if (argc == 1) cfg.load("config.csv");
  cfg.servos();
  verbose = cfg.flag("guicmdr.verbose", false);
  if (verbose) cfg.show();

  commander = std::shared_ptr < Commander > (new Commander());

  safety=CreateSafetyClient(cfg.str("guicmdr.safety.publish"),cfg.str("safety.subscribe"),2);
  safety->safe(false);

  commander->publish = cfg.str("commander.publish");
  commander->subscribers = cfg.list("commander.subscribers");
  commander->rxTimeout = 1e6;
  commander->start();

  signal(SIGINT, quitproc);
  signal(SIGTERM, quitproc);
  signal(SIGQUIT, quitproc);

  Gtk::Main kit(argc,argv);
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("src/MojUI.glade");


  builder->get_widget_derived("main_window", gui);
  kit.run(*gui);
  gui->end();
  delete gui;

  return 0;
}

