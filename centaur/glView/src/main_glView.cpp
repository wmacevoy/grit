//#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <zmq.h>
#include <math.h>
#include <string>
#include <signal.h>
#include "now.h"
#include "Configure.h"

Configure cfg;
bool verbose = false;

const int SCREENWIDTH = 300;
const int SCREENHEIGHT = 300;

std::string address = "";

GLint id;
GLint circle_points = 1081;
double r = 5;

void* context_lidar;
void* sub_lidar;

int64_t* lidar_data = NULL;
int sz_lidar_data = 1081;

int hwm = 1;
int linger = 25;

void subLidar() {
	static float t1 = 0, t2 = 0, timeOut = 0.5;
	int rcc;

	rcc = zmq_recv(sub_lidar, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);	
	if(rcc == sz_lidar_data) {
		t1 = now();
	}


	t2 = now();
	if(t2 - t1 > timeOut) {
		zmq_close(sub_lidar);
		sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);		
		if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
			if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
				if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
					if(zmq_connect(sub_lidar, address.c_str()) == 0) {
						std::cout << "Connection successfully set/reset" << std::endl;
						t1 = now();
					}

				}

			}
		}
	}	

	glutPostRedisplay();
}

// Scale is 10px/ft
void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	double angle = (3 * M_PI) / (2 * circle_points) ;
	glColor3f(0.2, 0.5, 0.5 );

	glBegin(GL_POINTS);
	double angle1 = 7 * M_PI / 4;
	glVertex2d(r * cos(0.0), r * sin(0.0));
	int i;
	for (i = circle_points; i >= 0; --i) {
		r = lidar_data[i];
		if(verbose) printf( "angle = %f \n" , angle1);
		glVertex2d(r * cos(angle1), r * sin(angle1));
		angle1 += angle ;
	}
	glEnd();		
	glFlush();

	glBegin(GL_LINE_LOOP);
	glVertex2f( -1, 1 );
	glVertex2f( -1, -1 );
	glVertex2f( 1, -1 );
	glVertex2f( 1, 1 );
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
	if(key == 27) {
		glutDestroyWindow(id);
		glutLeaveMainLoop();
	}
}

void quitproc(int param) {
	glutDestroyWindow(id);
	glutLeaveMainLoop();
}

int main( int argc,char **argv) {
	glutInit(&argc, argv);

	cfg.path("../../setup");
	cfg.args("glView.subscriber.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("glView.subscriber.verbose", false);
	if (verbose) cfg.show();

	address = cfg.str("glView.subscriber.address").c_str();

	signal(SIGINT, quitproc);
	signal(SIGTERM, quitproc);
	signal(SIGQUIT, quitproc);

	bool good = false;

	context_lidar = zmq_ctx_new ();
	sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	if(zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0) == 0) {
		if(zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm)) == 0) {
			if(zmq_setsockopt(sub_lidar, ZMQ_LINGER, &linger, sizeof(linger)) == 0) {
				if(zmq_connect(sub_lidar, address.c_str()) == 0) {
					glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
					glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
					glutInitWindowPosition(100, 100);
					glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
					id = glutCreateWindow("roboViewGL");
					init();
					glutIdleFunc(subLidar);
					glutKeyboardFunc(keyboard);
					glutDisplayFunc(draw);

					lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
					if(lidar_data) {
						good = true;					
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
