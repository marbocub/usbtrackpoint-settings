#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define TARGET_PID		0x6009
#define TARGET_VID	       	0x17ef

#define HID_GET_REPORT		0x01
#define HID_GET_IDLE		0x02
#define HID_GET_PROTOCOL	0x03
#define HID_SET_REPORT		0x09
#define HID_SET_IDLE		0x0A
#define HID_SET_PROTOCOL	0x0B

#define HID_REPORT_TYPE_INPUT	1
#define HID_REPORT_TYPE_OUTPUT	2
#define HID_REPORT_TYPE_FEATURE	3

#define HID_MAX_FIELDS 128

typedef struct {
	int	press_select;
	int	dragging;
	int	release_select;
	int	press_left;
	uint8_t	sensitivity;
	uint8_t	press_speed;
} USBTP_CONFIG;

void usbtp_setup_defaults (USBTP_CONFIG *usbtp)
{
	usbtp->press_select	= 0;
	usbtp->dragging		= 0;
	usbtp->release_select	= 0;
	usbtp->press_left	= 0;
	usbtp->sensitivity	= 160;
	usbtp->press_speed	= 56;
}

int usbtp_commit (libusb_device_handle *handle, USBTP_CONFIG *usbtp)
{
	int		ret;
	uint16_t	length, report_id, interface;
	uint8_t		data[HID_MAX_FIELDS];

	report_id = 4;
	interface = 1;
	data[0]   = 4;
	data[1]   = usbtp->press_select   ? 0x01 : 0x02;
	data[1]  |= usbtp->dragging       ? 0x04 : 0x08;
	data[1]  |= usbtp->release_select ? 0x10 : 0x20;
	data[1]  |= usbtp->press_left     ? 0x80 : 0x40;
	data[2]   = 0x03;
	data[3]   = usbtp->sensitivity;
	data[4]   = usbtp->press_speed;
	length    = 5;

	ret = libusb_control_transfer (handle,
				       LIBUSB_ENDPOINT_OUT
				       | LIBUSB_REQUEST_TYPE_CLASS
				       | LIBUSB_RECIPIENT_INTERFACE,
				       HID_SET_REPORT,
				       HID_REPORT_TYPE_FEATURE << 8 | report_id,
				       interface,
				       data,
				       length,
				       5000);

	return ret;
}

int main (int argc, char *argv[])
{
	libusb_context	*ctx = NULL;
	libusb_device_handle *handle = NULL;
	int		kernel_driver = 0;

	USBTP_CONFIG	usbtp;
	long		lvalue = 0;
	char 		*e;
	int		err = 0;

	usbtp_setup_defaults (&usbtp);

	if (argc > 1) {
		lvalue = strtol(argv[1], &e, 0);
		if (lvalue <= 0 || lvalue > 255 || *e != '\0') {
			lvalue = 0;
		}
	}
	if (lvalue != 0)
		usbtp.sensitivity = lvalue;

	err = libusb_init(&ctx);
	if (err < 0) {
		fprintf (stderr, "libusb init was failed\n");
		exit (1);
	}
//	libusb_set_debug(ctx, 3);

	handle = libusb_open_device_with_vid_pid (ctx, TARGET_VID, TARGET_PID);
	if (handle) {
		if ((kernel_driver = libusb_kernel_driver_active (handle, 1)) == 1) {
			libusb_detach_kernel_driver (handle, 1);
		}
		libusb_claim_interface (handle, 1);

		err = usbtp_commit (handle, &usbtp);
		if (err >= 0) {
			fprintf (stderr, "success: %d\n", usbtp.sensitivity);
		} else {
			fprintf (stderr, "error: %d\n", err);
		}

		libusb_release_interface (handle, 1);
		if (kernel_driver == 1) {
			libusb_attach_kernel_driver (handle, 1);
		}
		libusb_close (handle);
	} else {
		fprintf (stderr, "Device not found.\n");
	}

	libusb_exit(ctx);
	return 0;
}
