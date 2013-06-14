/*
 * This is the subscriber for the kinect.
 * Written by Michaela Ervin & Karl Castleton and using modifications from the glview example from libfreenect
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <zmq.h>

#define LONG  int
#define DWORD unsigned int
#define WORD unsigned short

const int sz_img = 640*480*3;

int saveImage = 0;

int g_argc;
char **g_argv;

int window;

void* sub_color;
void* sub_depth;

uint8_t* img_color;
uint8_t* img_depth;

GLuint gl_depth_tex;
GLuint gl_rgb_tex;

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

void CaptureScreen(int Width,int Height,uint8_t *image,char *fname,int fcount);

void subscribe_color(void *zmq_sub) 
{
	static int fcount = 0;

	printf("waiting...\n");

	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_msg_recv(&msg, zmq_sub, 0);
	memcpy(img_color, zmq_msg_data(&msg), sz_img);

	printf("received!\n");
	
	if(img_color != NULL && saveImage)
	{
		CaptureScreen(640, 480, img_color, "color_", fcount);
		fcount++; 
		saveImage = 0;
	}
}

void subscribe_depth(void *zmq_sub) 
{
	static int fcount = 0;

	printf("waiting...\n");

	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_msg_recv(&msg, zmq_sub, 0);
	memcpy(img_depth, zmq_msg_data(&msg), sz_img);

	printf("received!\n");
	
	if(img_depth != NULL && saveImage)
	{
		CaptureScreen(640, 480, img_depth, "depth_", fcount);
		fcount++; 
		saveImage = 0;
	}
}


///////////////////////////////////////////////////////OpenGL START
void DrawGLScene()
{
	subscribe_color(sub_color);
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

	glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, img_color);
	//glTexImage2D(GL_TEXTURE_2D, 0, 1, 640, 480, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img_color+640*4);

	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0); glVertex3f(640,0,0);
	glTexCoord2f(1, 0); glVertex3f(1280,0,0);
	glTexCoord2f(1, 1); glVertex3f(1280,480,0);
	glTexCoord2f(0, 1); glVertex3f(640,480,0);
	glEnd();

	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{

	if (key == 'c'){
		saveImage = 1;
	}
	if (key == 27) {
		glutDestroyWindow(window);
		// Not pthread_exit because OSX leaves a thread lying around and doesn't exit
		exit(0);
	}
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0,0,Width,Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 1280, 480, 0, -1.0f, 1.0f);
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

void *gl_threadfunc(void *arg)
{
	printf("GL thread\n");

	glutInit(&g_argc, g_argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1280, 480);
	glutInitWindowPosition(0, 0);

	window = glutCreateWindow("LibFreenect");

	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);

	InitGL(1280, 480);

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

int main(int argc, char** argv)
{
	int quit = 0;

	void *context_color = zmq_ctx_new ();
	void *context_depth = zmq_ctx_new ();

	sub_color = zmq_socket(context_color, ZMQ_SUB);
	sub_depth = zmq_socket(context_depth, ZMQ_SUB);

	if (zmq_connect(sub_color, "tcp://localhost:5556") !=0 || zmq_connect(sub_depth, "tcp://localhost:5557") !=0 )
	{

	}

	int rcc = zmq_setsockopt (sub_color, ZMQ_SUBSCRIBE, "", 0);
	int rcd = zmq_setsockopt (sub_depth, ZMQ_SUBSCRIBE, "", 0);
	assert (rcc == 0 && rcd == 0);
	
	img_color = (uint8_t*)malloc(sz_img);
	img_depth = (uint8_t*)malloc(sz_img);

	gl_threadfunc(NULL);

	/*//Main Loop
	while(!quit)
	{
		subscribe(sub);
		sleep(1);
	}*/

	zmq_close (sub_color);
	zmq_ctx_destroy (context_color);

	zmq_close (sub_depth);
	zmq_ctx_destroy (context_depth);

	return 0;
}
	
