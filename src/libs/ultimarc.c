




/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/usb.h>
#include <linux/usbdevice_fs.h>


#include <json/json.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include "ultimarc.h"

/*
struct usbdevfs_ctrltransfer {
        __u8   bRequestType;
        __u8   bRequest;
        __u16  wValue;
        __u16  wIndex;
        __u16  wLength;
        __u32  timeout;
        void   *data;
};

*/

#define REQUEST_TYPE 	 USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_MASK


void foo(void)
{
	puts("Ulimarc shared library test");
}

int write_to_device (char *device, char *data, int len)
{
	int fd, res, pos = 0;
	char buf[5];
	struct usbdevfs_ctrltransfer control;

	/* Open the Device with non-blocking reads. In real life,
	   don't use a hard coded path; use libudev instead. */
	fd = open(device, O_RDWR|O_NONBLOCK);

	if (fd < 0) {
		puts("Unable to open device");
		return 1;
	}

	// REQUEST_TYPE needs to equal 0x21
	control.bRequestType = REQUEST_TYPE;
	control.bRequestType = 0x21;
	control.bRequest = 0x09;
	control.wValue = 0x0203;
	control.wIndex = 0x02;
	control.wLength = 0x05;
	control.timeout = 500;
	control.data = &buf;

	while (pos < len)
    {
	   memcpy(&buf[0], &data[pos], 5);
	   pos+=5;

	   res = ioctl(fd, USBDEVFS_CONTROL, &control);

	   if (res < 0) {
		   printf("ioctl USBDEVFS_CONTROL returned: %d\n", res);
		   break;
	   } else {
		   printf("write() wrote %d bytes\n", res);
	   }

    }



	close(fd);

	return 0;


}

int validate_json (char *json)
{

	json_object *jobj = json_tokener_parse(json);
	//json_parse(jobj);

	enum json_type type;

	json_object_object_foreach(jobj, key, val) {

		type = json_object_get_type(val);

		switch (type) {

		case json_type_boolean:
			printf("type: json_type_boolean, ");
			printf("value: %sn", json_object_get_boolean(val)? "true": "false");
			break;
		}
	 }

	return 1;
}
