/*
 * This is the publisher for the kinect.
 * Written by Michaela Ervin, some* code taken from the glview example from libfreenect
 * 
 * Pressing ctrl-c in the terminal will cause normal cleanup and exit.
 * Any SIGTERM or SIGINT will cause normal cleanup and exit.
 *lots
*/

#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <assert.h>
#include <signal.h>
#include <pthread.h>
#include <chrono>
#include <math.h>
#include <zmq.h>
#include "urg_ctrl.h"
#include "libfreenect.h"
#include "Configure.h"

#if _WIN32
#pragma comment(lib, "pthreadVC2.lib")
#else
#include <unistd.h>
#endif

#define LONG  int
#define DWORD unsigned int
#define WORD unsigned short

Configure cfg;
bool verbose;

pthread_t freenect_thread;
volatile int die = 0;

int sleep_time;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
uint8_t *depth_mid = NULL;
uint8_t *rgb_back = NULL, *rgb_mid = NULL;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_led;

//Types = FREENECT_VIDEO_RGB or FREENECT_VIDEO_IR_8BIT;
freenect_video_format current_format = FREENECT_VIDEO_RGB;

pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frame_cond = PTHREAD_COND_INITIALIZER;

const int sz_img_color = 640*480*3;
const int sz_img_gray = 640*480;

uint16_t t_gamma[2048];

void publish_img(uint8_t* image, void* zmq_pub)
{
	int rc = zmq_send(zmq_pub, image, sz_img_color, ZMQ_DONTWAIT);
}

void depth_cb(freenect_device* dev, void* v_depth, uint32_t timestamp)
{
	/*static int count = 5;
	if(count > 0)
	{
		--count;
		return;
	}
	else
		count = 5;*/

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

void rgb_cb(freenect_device* dev, void* rgb, uint32_t timestamp)
{
  	/*static int count = 5;
	if(count > 0)
	{
		--count;
		return;
	}
	else
		count = 5;*/
	
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
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	if(!die) die = 1;

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
	printf("\nQuitting...\n");
	die = 1;
}

int main(int argc, char** argv)
{
	cfg.path("../../setup");
	cfg.args("kinect.provider.", argv);
	if (argc == 1) cfg.load("config.csv");
	verbose = cfg.flag("kinect.provider.verbose", false);
	if (verbose) cfg.show();
	
	sleep_time = cfg.num("kinect.requester.sleep_time", 25);

	int res;
	int hwm = 1;
	int rcc = 0;
	int rcd = 0;

	//Setup ZMQ and allocate memory buffers
	//tcp://*:9998 tcp://*:9999
	void* context_color = zmq_ctx_new ();	
	void* context_depth = zmq_ctx_new ();

	void* pub_color = zmq_socket(context_color, ZMQ_PUB);
	void* pub_depth = zmq_socket(context_depth, ZMQ_PUB);

	rcc = zmq_setsockopt(pub_color, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	rcd = zmq_setsockopt(pub_depth, ZMQ_SNDHWM, &hwm, sizeof(hwm));
	assert (rcc == 0 && rcd == 0);

	rcc = zmq_bind(pub_color, "tcp://*:9998");
	rcd = zmq_bind(pub_depth, "tcp://*:9999");

	for (int i = 0; i < 2048; ++i) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}

	//Initialize freenect
	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}

	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return 1;
	}

	//Open kinect device
	if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	//Allocate memory buffers
	depth_mid = (uint8_t*)malloc(sz_img_color);
	rgb_back = (uint8_t*)malloc(sz_img_color);
	rgb_mid = (uint8_t*)malloc(sz_img_color);
	assert(depth_mid != NULL && rgb_back != NULL && rgb_mid != NULL);

	//Start freenect thread
	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

#if !_WIN32
	struct sigaction new_action;
	new_action.sa_handler = SignalHandler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction (SIGTERM, &new_action, NULL);
	sigaction (SIGINT, &new_action, NULL);
#endif

	//Sleep for 1 second to allow thread to initialize
	std::this_thread::sleep_for(std::chrono::seconds(1));


	//Main loop
	printf("Publishing on tcp://*:9998 and tcp://*:9999\n");
	while(!die)
	{
		pthread_mutex_lock(&buf_mutex);

		//Send image via zmq
		publish_img(rgb_mid, pub_color);
		publish_img(depth_mid, pub_depth);

		pthread_cond_signal(&frame_cond);
		pthread_mutex_unlock(&buf_mutex);

		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	}

	//Cleanup

	pthread_join(freenect_thread, NULL);

	printf("freeing memory for images...\n");
	
	free(depth_mid);
	free(rgb_back);
	free(rgb_mid);

	printf("--done\n");
	printf("closing and destroying zmq\n");

	zmq_close(pub_color);
	zmq_close(pub_depth);

	zmq_ctx_destroy(context_color);
	zmq_ctx_destroy(context_depth);

	printf("-- done!\n");

	return 0;
}
