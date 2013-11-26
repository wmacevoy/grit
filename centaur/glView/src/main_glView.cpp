/*
 *  Layout:
 *
 *               15F 
 *            _______
 *            |     |
 *       -15L |  B  | 15R    
 *            |     |
 *            _______
 *              -15B
 *
 * Buddy is always orientated towards F
 * his neck (red line) can orient +-175 L and R
 *
*/
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <zmq.h>
#include <math.h>
#include <string>
#include <signal.h>
#include <thread>
#include <chrono>
#include <memory>
#include "now.h"
#include "Configure.h"
#include "CreateZMQServoListener.h"

Configure cfg;
bool verbose = false;
int sleep_time;
std::shared_ptr<ServoController> servos;
Servo *neckud;
Servo *necklr;

const int SCREENWIDTH = 300;
const int SCREENHEIGHT = 300;

std::string addressL = "";

void* context_lidar;
void* sub_lidar;

int hwm = 1;
int linger = 25;

int64_t* lidar_data = NULL;
int sz_lidar_data = 1081;

double r = 5.0;
double neckAngle;
const GLint circle_points = 1081;
const double arcStep = (3 * M_PI) / (2 * circle_points);
const double neckAdjust = 3 * M_PI / 4;

struct Point {
 double x, y;
 Point(double _X, double _Y) {
	x = _X; y = _Y;
 }
};

struct Bot {
 Point tl;
 Point bl;
 Point br;
 Point tr;
 Bot() : tl(-1.4, 1.4), bl(-1.4, -1.4), br(1.4, -1.4), tr(1.4, 1.4){}
};


void getData() {
	static float tl1 = 0, tl2 = 0, timeOut = 0.6;
	int rcc;

	rcc = zmq_recv(sub_lidar, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);	
	if(rcc > 0) {
		tl1 = now();
	}

	neckAngle = necklr->angle();

	//Reset sub_lidar if no valid packet is received in 0.5 seconds
	tl2 = now();
	if(tl2 - tl1 > timeOut) {
		zmq_close(sub_lidar);
		sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);
		if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {	
			if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
					if(zmq_connect(sub_lidar, addressL.c_str()) == 0) {
						std::cout << "Connection to lidar successfully set/reset" << std::endl;
						tl1 = now();
					}

				}

			}
		}
	}	

	glutPostRedisplay();
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
}

//Scale is 10px/ft
//Red line is orientation of the neck/head
//Green object in center is buddy, positioned always at (0,0) and oriented 'forward'
//Black rays get drawn to the point of detection, screen does not refresh.  This way
//a map of what the bot is seeing and has seen is made
//glVertex2d coordinates are given in feet.
void draw() {
	//glClear(GL_COLOR_BUFFER_BIT);

	int i;
	double neckYaw = (neckAngle * (M_PI / 180)) + (M_PI / 2);
	double angle = neckYaw - neckAdjust;
	double rcos, rsin;	
	
	for (i = 0; i < circle_points; ++i) {
		r = lidar_data[i] * 0.00328084;

		if(verbose) printf( "angle = %f \n" , angle);

		rcos = r * cos(angle);
		rsin = r * sin(angle);		

		glColor3f(0.0, 0.0, 0.0); //Black
		glBegin(GL_LINES);
		glVertex2d(0, 0);
		glVertex2d(rcos, rsin);
		glEnd();

		glColor3f(0.2, 0.5, 0.5); //Blue
		glBegin(GL_POINTS);
		glVertex2d(rcos, rsin);
		glEnd();

		angle += arcStep ;
	}		

	glColor3f(0.2, 0.5, 0.0); //Green-ish
	glBegin(GL_LINE_LOOP);
	glVertex2f( -1.4, 1.4 );
	glVertex2f( -1.4, -1.4 );
	glVertex2f( 1.4, -1.4 );
	glVertex2f( 1.4, 1.4 );
	glEnd();
	
	glBegin(GL_LINES);
	glVertex2f( 0, 1 );
	glVertex2f( 0, 2 );
	glEnd();

	glColor3f(0.8, 0.0, 0.0); //Red
	glBegin(GL_LINES);
	glVertex2f( 0, 0 );
	glVertex2f( 2 * cos(neckYaw), 2 * sin(neckYaw) );
	glEnd();

	glutSwapBuffers();
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-15.0, 15.0, -15.0, 15.0, -1.0, 1.0);
}

void keyboard (unsigned char key , int x, int y) {
	if(key == 27 || key == 'q') {
		glutLeaveMainLoop();
	}
}

void quitproc(int param) {
	glutLeaveMainLoop();
}

int main( int argc,char **argv) {
	cfg.path("../../setup");
	cfg.args("glView.subscriber.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("glView.subscriber.verbose", false);
	if (verbose) cfg.show();

	servos = std::shared_ptr < ServoController > (CreateZMQServoListener(cfg.str("servos.subscribe")));
	neckud=servos->servo(94);
	necklr=servos->servo(93);
	servos->start();

	addressL = cfg.str("glView.subscriber.addressL").c_str();
	//	addressN = cfg.str("glView.subscriber.addressN").c_str();
	sleep_time = (int)cfg.num("glView.subscriber.sleep_time");

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	bool good = false;

	context_lidar = zmq_ctx_new ();
	sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
	  if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
	    if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
	      if(zmq_connect(sub_lidar, addressL.c_str()) == 0) {
					glutInit(&argc, argv);
					glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
					glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
					glutInitWindowPosition(100, 100);
					glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
					glutCreateWindow("roboViewGL");
					init();
					glutIdleFunc(getData);
					glutKeyboardFunc(keyboard);
					glutDisplayFunc(draw);

					lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
					if(lidar_data) {
						good = true;					
						std::cout << "Everthing was successfully initialized" <<std::endl;					
					}
				}
			}
		}
	}

	if(good) glutMainLoop();

	std::cout << std::endl << "Quitting..." << std::endl;
	std::cout << "freeing memory..." << std::endl;
	if(lidar_data) free(lidar_data);
	std::cout << "closing and destroying zmq..." << std::endl;
	zmq_close(sub_lidar);
	zmq_ctx_destroy(context_lidar);
	std::cout << "--done!" << std::endl;
	return 0; 
}
