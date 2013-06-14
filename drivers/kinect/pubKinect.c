/*
 * This is the publisher for the kinect.
 * Modified by Karl Castleton & Michaela Ervin from the glview example from libfreenect
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zmq.h>
#include <assert.h>
#include <signal.h>

#include "libfreenect.h"

#include <pthread.h>

#include <math.h>

#define LONG  int
#define DWORD unsigned int
#define WORD unsigned short

pthread_t freenect_thread;
volatile int die = 0;
volatile int quit = 0;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
uint8_t *depth_mid;
uint8_t *rgb_back, *rgb_mid;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_led;

//freenect_video_format requested_format = FREENECT_VIDEO_RGB;
freenect_video_format current_format = FREENECT_VIDEO_RGB;

pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frame_cond = PTHREAD_COND_INITIALIZER;

const int sz_img = 640*480*3;

uint16_t t_gamma[2048];

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

void publish_obj(char obj, void *zmq_pub)
{
	int rc = zmq_send(zmq_pub, &obj, sizeof(char), ZMQ_DONTWAIT);
}

void publish_img(uint8_t *image, void *zmq_pub)
{
	int rc = zmq_send(zmq_pub, image, sz_img, ZMQ_DONTWAIT);
}

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i;
	uint16_t *depth = (uint16_t*)v_depth;

	pthread_mutex_lock(&buf_mutex);
	for (i=0; i<640*480; i++) {
		int pval = t_gamma[depth[i]];
		int lb = pval & 0xff;
		switch (pval>>8) {
			case 0:
				depth_mid[3*i+0] = 255;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+2] = 255-lb;
				break;
			case 1:
				depth_mid[3*i+0] = 255;
				depth_mid[3*i+1] = lb;
				depth_mid[3*i+2] = 0;
				break;
			case 2:
				depth_mid[3*i+0] = 255-lb;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+2] = 0;
				break;
			case 3:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+2] = lb;
				break;
			case 4:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+2] = 255;
				break;
			case 5:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+2] = 255-lb;
				break;
			default:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+2] = 0;
				break;
		}
	}

	pthread_cond_signal(&frame_cond);
	pthread_mutex_unlock(&buf_mutex);
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	pthread_mutex_lock(&buf_mutex);

	// swap buffers
	assert (rgb_back == rgb);
	rgb_back = rgb_mid;
	freenect_set_video_buffer(dev, rgb_back);
	rgb_mid = (uint8_t*)rgb;

	pthread_cond_signal(&frame_cond);
	pthread_mutex_unlock(&buf_mutex);
}

void *freenect_threadfunc(void *arg)
{
	freenect_set_led(f_dev,LED_GREEN);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_video_callback(f_dev, rgb_cb);
	freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, current_format));
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
	freenect_set_video_buffer(f_dev, rgb_back);

	freenect_start_depth(f_dev);
	freenect_start_video(f_dev);

	while (!die && freenect_process_events(f_ctx) >= 0) {
		//Need to get *nix command for sleep in milliseconds
	}

	printf("shutting down streams...\n");

	freenect_set_led(f_dev, LED_BLINK_GREEN);

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);

	printf("-- done!\n");
	return NULL;
}

void SignalHandler(int sig)
{
	die = 1;
	quit = 1;
}

int main(int argc, char** argv)
{
	int res;
	//tcp://*:5556 tcp://*:5557 tcp://*:5558
	//void *context_obj = zmq_ctx_new ();
	//void *pub_obj = zmq_socket(context_obj, ZMQ_PUB);
	//int rco = zmq_bind(pub_obj, "tcp://*:5558");

	void *context_color = zmq_ctx_new ();
	void *pub_color = zmq_socket(context_color, ZMQ_PUB);
	int rcc = zmq_bind(pub_color, "tcp://*:5556");

	void *context_depth = zmq_ctx_new ();
	void *pub_depth = zmq_socket(context_depth, ZMQ_PUB);
	int rcd = zmq_bind(pub_depth, "tcp://*:5557");	

	depth_mid = (uint8_t*)malloc(640*480*3);
	rgb_back = (uint8_t*)malloc(640*480*3);
	rgb_mid = (uint8_t*)malloc(640*480*3);

	//Initialize
	int i;
	for (i=0; i<2048; i++) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}

	//freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;
	if (argc > 1) {
		user_device_number = atoi(argv[1]);
		printf("%s", argv[1]);
	}

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return 1;
	}

	if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	struct sigaction new_action;
	new_action.sa_handler = SignalHandler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction (SIGTERM, &new_action, NULL);
	sigaction (SIGINT, &new_action, NULL);

	//Sleep for 1 second to allow thread to initialize
	sleep(1);

	while(1)
	{
		if(quit == 1)
		{
			break;
		}

		pthread_mutex_lock(&buf_mutex);

		//Send image data via zmq
		publish_img(rgb_mid, pub_color);
		publish_img(depth_mid, pub_depth);
		
		pthread_cond_signal(&frame_cond);
		pthread_mutex_unlock(&buf_mutex);

	}

	//Cleanup
	printf("Quitting...\n");

	pthread_join(freenect_thread, NULL);
	
	free(depth_mid);
	free(rgb_back);
	free(rgb_mid);

	printf("memory for images is free\n");

	//zmq_close(pub_obj);
	zmq_close(pub_color);
	zmq_close(pub_depth);

	//zmq_ctx_destroy(context_obj);
	zmq_ctx_destroy(context_color);
	zmq_ctx_destroy(context_depth);

	printf("zmq is closed and destroyed\n");

	printf("-- done!\n");

	return 0;
}
