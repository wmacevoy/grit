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

const int CHARSPERLINE = 64;

class guicmdr : public Gtk::Window
{
protected:
  Glib::RefPtr<Gtk::Builder> builder;
  Gtk::Button *send, *f, *b, *r, *l, *sr, *sl, *h;
	Glib::RefPtr<Gtk::EntryBuffer> cmdBuf;
	Gtk::Entry *ent_cmd;
	Glib::RefPtr<Gtk::TextBuffer> tb_old, tb_resp;
	Gtk::TextView *tv_old, *tv_resp;
	Gtk::Image *img_on, *img_off;
	Glib::ustring text;

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
		builder->get_widget("command", ent_cmd);
		builder->get_widget("oldCommands", tv_old);
		builder->get_widget("response", tv_resp);
		builder->get_widget("safeOn", img_on);
		builder->get_widget("safeOff", img_off);

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

		img_off->hide();
		img_on->hide();
	}

	~guicmdr(){}

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

  Gtk::Main kit(argc,argv);
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("src/MojUI.glade");

  builder->get_widget_derived("main_window", gui);
  kit.run(*gui);
  gui->end();

	return 0;
}
