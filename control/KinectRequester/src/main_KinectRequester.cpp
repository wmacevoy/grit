/*
 * This is the subscriber for the kinect.
 * Written by Michaela Ervin & Karl Castleton and using modifications from the glview example from libfreenect
 * 
 * Press:
 * t - toggle view
 * c - capture image
*/

#include <HokuyoProviderRequest.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <thread>
#include <chrono>
#include <zmq.h>
#include <atomic>
#include <iomanip>
#include <mutex>

using namespace std;

#define LONG  int
#define DWORD unsigned int
#define WORD unsigned short

const int sz_img = 640*480*3;
const int nScans = 1;

int saveImagec;
int saveImaged;

int g_argc;
char** g_argv;

int window;
int view_state;

void* sub_color;
void* sub_depth;

void* context_color;
void* context_depth;

uint8_t* img_color;
uint8_t* img_depth;

std::atomic<int> mx, my;

std::thread* hThread;

GLuint gl_depth_tex;
GLuint gl_rgb_tex;

HokuyoData data;
std::mutex locker;

std::atomic<bool> hokuyoThreadRunning;
std::atomic<bool> getHData;

std::string providerAddress;

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

void getData()
{
	while(hokuyoThreadRunning)
	{
	  if(getHData)
	  {
		locker.lock();	
		data = HokuyoProviderRequest::GetData(providerAddress.c_str(), nScans);
		locker.unlock();
	  }
	  std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	
}

///////////////////////////////////////////////////////SUBSCRIBE START 
void subscribe_color(void *zmq_sub) 
{
	static int fcount = 0;

	//printf("waiting...\n");

	int rc = zmq_recv(zmq_sub, img_color, sz_img, ZMQ_DONTWAIT);

	//printf("received!\n");
	
	if(saveImagec && img_color != NULL)
	{
		CaptureScreen(640, 480, img_color, "color_", fcount);
		fcount++; 
		saveImagec = 0;
	}
}

void subscribe_depth(void *zmq_sub) 
{
	static int fcount = 0;

	//printf("waiting...\n");

	int rc = zmq_recv(zmq_sub, img_depth, sz_img, ZMQ_DONTWAIT);

	//printf("received!\n");
	
	if(saveImaged && img_depth != NULL)
	{
		CaptureScreen(640, 480, img_depth, "depth_", fcount);
		fcount++; 
		saveImaged = 0;
	}
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
	if( x >= 0 && x <= 640 && y >= 240 && y <= 250)
	{
		getHData = true;
		int tmpX = x;
		std::string pos;
		
		if(data.m_error.empty())
		{
			int index = 425 + ((x * 229) / 625);
			locker.lock();
			pos = convstr(data.m_dataArrayArray[0][index] * 0.00328084f);
			locker.unlock();
		}
		else
		{
			locker.lock();
			pos = data.m_error;
			locker.unlock();
		}

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
	else
	{
		getHData = false;
	}
}

void DrawGLScene()
{
	switch(view_state)
	{
	case 0:
		subscribe_depth(sub_depth);	

		glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, img_depth);

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 1); glVertex3f(640,480,0);
		glTexCoord2f(0, 1); glVertex3f(0,480,0);
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
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, img_color);

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0, 0); glVertex3f(0,0,0);
		glTexCoord2f(1, 0); glVertex3f(640,0,0);
		glTexCoord2f(1, 1); glVertex3f(640,480,0);
		glTexCoord2f(0, 1); glVertex3f(0,480,0);
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
	glOrtho (0, 640, 480, 0, -1.0f, 1.0f);
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
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);

	window = glutCreateWindow("ICU");

	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	glutPassiveMotionFunc(&MousePos);

	InitGL(640, 480);

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
			bf.bfSize	 = sizeof(bf)+sizeof(bi)+Width*Height*3;
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
			fwrite( image, sizeof(unsigned char), Height*Width*3, file );

			fclose( file );
		}
	}
}

void bye()
{
	printf("Quitting...\n");

	getHData = false;
	hokuyoThreadRunning = false;
	hThread->join();
	delete hThread;

	printf("freeing memory for images...\n");

	free(img_color);
	free(img_depth);

	printf("-- done!\n");
	printf("shutting down zmq...\n");

	zmq_close(sub_color);
	zmq_close(sub_depth);

	zmq_ctx_destroy(context_color);
	zmq_ctx_destroy(context_depth);

	printf("-- done!\n");
}

int main(int argc, char** argv)
{
	int quit = 0;
	int hwm = 1;
	int rcc = 0;
	int rcd = 0;
	char ip1[40];
	char ip2[40];

	strcpy(ip1, "tcp://");
	strcpy(ip2, "tcp://");

	if(argc > 1)
	{
		strcat(ip1, argv[1]);
		strcat(ip1, ":");
		strcat(ip2, argv[1]);
		strcat(ip2, ":");
		g_argc = argc;
		g_argv = argv;
		providerAddress = "tcp://";
		providerAddress.append(argv[1]);
	}
	else
	{
		strcat(ip1, "localhost:");
		strcat(ip2, "localhost:");
		providerAddress = "tcp://";
		providerAddress.append("localhost");
	}

	strcat(ip1, "9998\0");
	strcat(ip2, "9999\0");

	printf("Listening on: %s, %s\n", ip1, ip2);

	context_color = zmq_ctx_new ();
	context_depth = zmq_ctx_new ();

	sub_color = zmq_socket(context_color, ZMQ_SUB);
	sub_depth = zmq_socket(context_depth, ZMQ_SUB);

	rcc = zmq_setsockopt(sub_color, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	rcd = zmq_setsockopt(sub_depth, ZMQ_RCVHWM, &hwm, sizeof(hwm));
	assert (rcc == 0 && rcd == 0);

	rcc = zmq_setsockopt(sub_color, ZMQ_SUBSCRIBE, "", 0);
	rcd = zmq_setsockopt(sub_depth, ZMQ_SUBSCRIBE, "", 0);
	assert (rcc == 0 && rcd == 0);

	img_color = (uint8_t*)malloc(sz_img);
	img_depth = (uint8_t*)malloc(sz_img);

	//tcp://localhost:9998  tcp://localhost:9999
	if (zmq_connect(sub_color, ip1) !=0 || zmq_connect(sub_depth, ip2) !=0)
	{
		printf("Error initializing 0mq...\n");
		exit(1);
	}

	saveImagec = 0;
	saveImaged = 0;

	view_state = 1;

	mx = my = 0;	
	
	hokuyoThreadRunning = true;
	getHData = false;

	hThread = new std::thread(getData);

	gl_threadfunc(NULL);

	bye();

	return 0;
}
	
