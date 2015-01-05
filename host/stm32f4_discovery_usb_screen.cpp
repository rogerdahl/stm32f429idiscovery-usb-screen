//#include <unistd.h>
#include <cstdlib>
#include <cstdio>
//#include <errno.h>
//#include <signal.h>

extern "C" {
	#include "libusb-1.0/libusb.h"
}

static struct libusb_device_handle* devh = NULL;

#define VIDEO_ENDPOINT_OUT 0x01

// 320 * 240 * 2 = 153,600
// 153600 / 4 = 38400 (fits in 16 bit USB transfer size)
#define SCREEN_BLOCK_SIZE_BYTES 38400

void send_screen_block(uint16_t* screen_buf, int block_idx);

int init_usb()
{
	int rc = libusb_init(NULL);
	if (rc < 0) {
		fprintf(stderr, "Error initializing libusb: %s\n", libusb_error_name(rc));
		return -1;
	}

	//libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);

	devh = libusb_open_device_with_vid_pid(NULL, 0x6666, 0x7777);
	if (!devh) {
		fprintf(stderr, "Error finding USB device\n");
		goto error;
	}

	rc = libusb_claim_interface(devh, 0);
	if (rc < 0) {
		fprintf(stderr, "Error claiming interface: %s\n", libusb_error_name(rc));
		goto error;
	}

	return rc;
		
error:
	if (devh) {
		libusb_close(devh);
	}
  
	libusb_exit(NULL);
  	
	return rc;
}

void deinit_usb()
{
	libusb_release_interface(devh, 0);
	libusb_close(devh);
	libusb_exit(NULL);
}

void send_screen(uint16_t* screen_buf)
{
	for (int i = 0; i < 4; ++i) {
		send_screen_block(screen_buf + (SCREEN_BLOCK_SIZE_BYTES / sizeof(uint16_t)) * i, i);
	}
}

void send_screen_block(uint16_t* screen_buf, int block_idx)
{
	libusb_control_transfer(devh, 0x41, 0, (uint16_t)block_idx, 0, 0, 0, 0);
	int transferred;
	int rc = libusb_bulk_transfer(devh, VIDEO_ENDPOINT_OUT, (unsigned char*)screen_buf, SCREEN_BLOCK_SIZE_BYTES, &transferred, 1000);
	if (rc < 0 || transferred < SCREEN_BLOCK_SIZE_BYTES) {
		fprintf(stderr, "Error: libusb_bulk_transfer failed: Error: %s, transferred: %d\n", libusb_error_name(rc), transferred);
		// TODO: Recover?
		// LIBUSB_ERROR_NO_DEVICE
		//libusb_reset_device(devh);
	}
}
