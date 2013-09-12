/*
 * This is the subscriber for the kinect.
 * Written by Michaela Ervin & Karl Castleton and using modifications from the glview example from libfreenect
 * 
 * Press:
 * t - toggle view
 * c - capture image
*/

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <thread>
#include <chrono>
#include <zmq.h>
#include <atomic>
#include <iomanip>
#include <mutex>
#include "urg_ctrl.h"
#include "Configure.h"

Configure cfg;
bool verbose;

#define LONG  int
#define DWORD unsigned int
#define WORD unsigned short

const int width = 640;
const int height = 480;

const int sz_img_color = width*height*3;
const int nScans = 1;
int sleep_time;

int saveImagec;
int saveImaged;

int g_argc;
char** g_argv;

int window;
int view_state;

void* sub_color;
void* sub_depth;
void* sub_lidar;

void* context_color;
void* context_depth;
void* context_lidar;

uint8_t* img_color;
uint8_t* img_depth;
int64_t* lidar_data;

const int sz_lidar_data  = 1081;

std::atomic<int> mx, my;

GLuint gl_depth_tex;
GLuint gl_rgb_tex;

std::mutex locker;
const int64_t time_threshold = 1000;


typedef struct __attribute__((packed)) tagBITMAPFILEHEADER
{
 WORD bfType;
 DWORD bfSize;
 WORD bfReserved1;
 WORD bfReserved2;
 DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
 DWORD biSize;
 LONG biWidth;
 LONG biHeight;
 WORD biPlanes;
 WORD biBitCount;
 DWORD biCompression;
 DWORD biSizeImage;
 LONG biXPelsPerMeter;
 LONG biYPelsPerMeter;
 DWORD biClrUsed;
 DWORD biClrImportant;
} BITMAPINFOHEADER;

void CaptureScreen(int Width, int Height, uint8_t *image, char *fname, int fcount);

///////////////////////////////////////////////////////SUBSCRIBE START 
void subscribe_color(void* zmq_sub) 
{
	static int fcount = 0;

	//if(verbose) printf("waiting for color image...\n");

	locker.lock();
	int rc = zmq_recv(zmq_sub, img_color, sz_img_color, ZMQ_DONTWAIT);
	locker.unlock();

	//if(verbose && rc > 0) printf("received color image!\n");
	
	if(saveImagec && img_color != NULL)
	{
		CaptureScreen(width, height, img_color, "color_", fcount);
		fcount++; 
		saveImagec = 0;
	}
}

void subscribe_depth(void* zmq_sub) 
{
	static int fcount = 0;

	//if(verbose) printf("waiting for depth image...\n");
	
	locker.lock();
	int rc = zmq_recv(zmq_sub, img_depth, sz_img_color, ZMQ_DONTWAIT);
	locker.unlock();

	//if(verbose && rc > 0) printf("received depth image!\n");
	
	if(saveImaged && img_depth != NULL)
	{
		CaptureScreen(width, height, img_depth, "depth_", fcount);
		fcount++; 
		saveImaged = 0;
	}
}

void subscribe_lidar(void* zmq_sub)
{
	if(verbose) printf("waiting for lidar data...\n");

	locker.lock();
	int rc = zmq_recv(zmq_sub, lidar_data, sz_lidar_data * sizeof(int64_t), ZMQ_DONTWAIT);
	locker.unlock();

	if(verbose && rc > 0) printf("received lidar data!\n");
}
///////////////////////////////////////////////////////SUBSCRIBE END

std::string convstr(const float t)
{
	std::stringstream ftoa;
	ftoa << std::setprecision(3) << std::setw(4) << t;
	return ftoa.str();
}

///////////////////////////////////////////////////////OpenGL START
void RenderString(float x, float y)
{
	//Kinect horizontal fov is 57 degrees, so 28.5 degrees left and 28.5 degrees right
	//lidar does 1081 points in 270 degrees, so 4 pts per degree, with 114 pts left and 114 pts right
	//540 is lidar center left side = 426 right side = 654
	if( x >= 0 && x <= width && y >= 240 && y <= 250)
	{
		subscribe_lidar(sub_lidar);
		
		int tmpX = x;
		std::string pos;
		
		int index = 425 + ((x * 229) / 625);  //Needs to be remapped

		locker.lock();
		pos = convstr(lidar_data[index] * 0.00328084f);
		locker.unlock();
		
		if(x > 550) tmpX = x - 55;		

		glColor3f(1.0f, 1.0f, 1.0f); 
		glRasterPos2f(tmpX + 1, y + 1);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)pos.c_str());
		glColor3f(1.0f, 1.0f, 1.0f); 
		glRasterPos2f(tmpX - 1, y - 1);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)pos.c_str());
		glColor3f(0.1f, 0.1f, 0.1f); 
		glRasterPos2f(tmpX, y);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)pos.c_str());
	}
}

void DrawGLScene()
{
	switch(view_state)
	{
	case 0:
		subscribe_depth(sub_depth);	

		glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_depth);

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(width,0,0);
		glTexCoord2f(1, 1); glVertex3f(width,height,0);
		glTexCoord2f(0, 1); glVertex3f(0,height,0);
		glEnd();

		//Left triangle
		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.1, 0.1);
		glVertex3f(0, 240, 0);
		glVertex3f(20, 245, 0);
		glVertex3f(0, 250, 0);
		glEnd();
		
		//Right triangle
		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.1, 0.1);
		glVertex3f(640, 240, 0);
		glVertex3f(620, 245, 0);
		glVertex3f(640, 250, 0);
		glEnd();

		//Top line
		glBegin(GL_LINES);
		glVertex3f(0, 240, 0);
		glVertex3f(640, 240, 0);
		glEnd();

		//Bottom line
		glBegin(GL_LINES);
		glVertex3f(0, 250, 0);
		glVertex3f(640, 250, 0);
		glEnd();

		RenderString(mx, my);
		break;

	case 1:
		subscribe_color(sub_color);
		
		glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_color);

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(width,0,0);
		glTexCoord2f(1, 1); glVertex3f(width,height,0);
		glTexCoord2f(0, 1); glVertex3f(0,height,0);
		glEnd();

		//Left triangle
		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.1, 0.1);
		glVertex3f(0, 240, 0);
		glVertex3f(20, 245, 0);
		glVertex3f(0, 250, 0);
		glEnd();
		
		//Right triangle
		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.1, 0.1);
		glVertex3f(640, 240, 0);
		glVertex3f(620, 245, 0);
		glVertex3f(640, 250, 0);
		glEnd();

		//Top line
		glBegin(GL_LINES);
		glVertex3f(0, 240, 0);
		glVertex3f(640, 240, 0);
		glEnd();

		//Bottom line
		glBegin(GL_LINES);
		glVertex3f(0, 250, 0);
		glVertex3f(640, 250, 0);
		glEnd();

		RenderString(mx, my);
		break;
	default:
		//Handle unknown state here.  Although it should never be unknown
		break;
	}

	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{

	if(key == 'c'){
		printf("c\n");
		if(view_state == 1){
			saveImagec = 1;
		}
		else if(view_state == 0){
			saveImaged = 1;
		}
	}
	else if(key == 't'){
		printf("t\n");
		if(view_state == 0){
			view_state = 1;
		}
		else if(view_state == 1){
			view_state = 0;
		}
	}
	else if(key == 27) {
		glutDestroyWindow(window);
		glutLeaveMainLoop();		
	}
}

void MousePos(int x, int y)
{
	mx = x;
	my = y;
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, width, height, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();
}

void InitGL(int Width, int Height)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
    	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);

	glGenTextures(1, &gl_depth_tex);
	glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &gl_rgb_tex);
	glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ReSizeGLScene(Width, Height);
}

void* gl_threadfunc(void* arg)
{
	glutInit(&g_argc, g_argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);

	window = glutCreateWindow("ICU");

	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	glutPassiveMotionFunc(&MousePos);

	InitGL(width, height);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();

	return NULL;
}
///////////////////////////////////////////////////////OpenGL END

void CaptureScreen(int Width,int Height,uint8_t *image,char *fname,int fcount)
{
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	char filename[255];

	sprintf(filename,"%s%d.bmp",fname,fcount);

	FILE *file	 = fopen(filename, "wb");

	if( image!=NULL )
	{
		if( file!=NULL ) 
		{
			memset( &bf, 0, sizeof( bf ) );
			memset( &bi, 0, sizeof( bi ) );


			bf.bfType	 = 'MB';
			bf.bfSize	 = sizeof(bf)+sizeof(bi)+Width*Height;
			bf.bfOffBits	 = sizeof(bf)+sizeof(bi);
			bi.biSize	 = sizeof(bi);
			bi.biWidth	 = Width;
			bi.biHeight	 = Height;
			bi.biPlanes	 = 1;
			bi.biBitCount	 = 24;
			bi.biSizeImage	 = Width*Height*3;

			fwrite( &bf, sizeof(bf), 1, file );
			fwrite( &bi, sizeof(bi), 1, file );
			int i;
			for (i=0;i<bi.biSizeImage;i+=3) {
				uint8_t c = image[i];
				image[i] = image[i+2];
				image[i+2] = c;
			}
			fwrite( image, sizeof(unsigned char), Height*Width, file );

			fclose( file );
		}
	}
}

void bye()
{
	if (verbose) printf("Quitting...\n");
	if (verbose) printf("freeing memory for images...\n");

	free(img_color);
	free(img_depth);
	free(lidar_data);

	if (verbose) printf("-- done!\n");
	if (verbose) printf("shutting down zmq...\n");

	zmq_close(sub_color);
	zmq_close(sub_depth);
	zmq_close(sub_lidar);

	zmq_ctx_destroy(context_color);
	zmq_ctx_destroy(context_depth);
	zmq_ctx_destroy(context_lidar);

	if (verbose) printf("-- done!\n");
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("kinect.requester.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("kinect.requester.verbose", false);
	if (verbose) cfg.show();

	sleep_time = cfg.num("kinect.requester.sleep_time", 25);
	std::string address = cfg.str("kinect.requester.address", "localhost");

	int quit = 0;
	int hwm = 1;
	int rcc = 0;
	int rcd = 0;
	int rcl = 0;
	char ip1[40];
	char ip2[40];
	char ip3[40];

	//Setup IP addresses
	strcpy(ip1, "tcp://");
	strcpy(ip2, "tcp://");
	strcpy(ip3, "tcp://");

	strcat(ip1, address.c_str());
	strcat(ip1, ":");
	strcat(ip2, address.c_str());
	strcat(ip2, ":");
	strcat(ip3, address.c_str());
	strcat(ip3, ":");
	g_argc = argc;
	g_argv = argv;

	strcat(ip1, "9998\0");
	strcat(ip2, "9999\0");
	strcat(ip3, "9997\0");

	if (verbose) printf("Listening on: %s, %s, %s\n", ip1, ip2, ip3);

	//Initialize ZMQ
	context_color = zmq_ctx_new ();
	context_depth = zmq_ctx_new ();
	context_lidar = zmq_ctx_new ();

	sub_color = zmq_socket(context_color, ZMQ_SUB);
	sub_depth = zmq_socket(context_depth, ZMQ_SUB);
	sub_lidar = zmq_socket(context_lidar, ZMQ_SUB);

	rcc = zmq_setsockopt(sub_color, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rcd = zmq_setsockopt(sub_depth, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rcl = zmq_setsockopt(sub_lidar, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert (rcc == 0 && rcd == 0 && rcl ==0);

	rcc = zmq_setsockopt(sub_color, ZMQ_SUBSCRIBE, "", 0);
	rcd = zmq_setsockopt(sub_depth, ZMQ_SUBSCRIBE, "", 0);
	rcl = zmq_setsockopt(sub_lidar, ZMQ_SUBSCRIBE, "", 0);
	assert (rcc == 0 && rcd == 0 && rcl == 0);

	//Allocate memory buffers
	img_color = (uint8_t*)calloc(sz_img_color, sizeof(uint8_t));
	img_depth = (uint8_t*)calloc(sz_img_color, sizeof(uint8_t));	
	lidar_data = (int64_t*)calloc(sz_lidar_data, sizeof(int64_t));
	assert(img_color != NULL && img_depth != NULL && lidar_data != NULL);

	//tcp://localhost:9998  tcp://localhost:9999 tcp://localhost:9997
	if (zmq_connect(sub_color, ip1) !=0 || zmq_connect(sub_depth, ip2) !=0 || zmq_connect(sub_lidar, ip3))
	{
		if (verbose) printf("Error initializing 0mq...\n");
		return 1;
	}

	saveImagec = 0;
	saveImaged = 0;

	view_state = 1;

	mx = my = 0;	

	gl_threadfunc(NULL);

	bye();

	return 0;
}
	
