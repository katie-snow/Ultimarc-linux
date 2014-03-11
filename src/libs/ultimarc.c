
/* Unix */
#include <linux/usbdevice_fs.h>
#include <json.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <linux/hiddev.h>


#include "ultimarc.h"


#define UM_CTRL_REQUEST_TYPE 0x21
#define UM_CTRL_REQUEST      0x9
#define UM_CTRL_VALUE        0x0203
#define UM_CTRL_INDEX        0x2
#define UM_CTRL_BUFFER_SIZE  0x5

#define DEV_VENDOR			 0xd208
#define DEV_PRODUCT			 0x0310

/*
static struct usb_device_id ultimarc_table[] =
{
		{ USB_DEVICE(0xd208, 0x0310) },    // IPAC-2, IPAC-4, IPAC-MINI
		{ USB_DEVICE(0xd209, 0x0501) },    // Ultra-Stick 360
		{ USB_DEVICE(0xd209, 0x1401) },    // PAC-LED 64
		{} // Terminating entry
};
*/

int lib_ready = 0;

void foo(void)
{
	puts("Ulimarc shared library test");
}

void initialize ()
{
	//usb_init();

	lib_ready = 1;
}

// IOCTL_USBFS_DISCONNECT

// https://www.kernel.org/doc/htmldocs/usb/API-usb-driver-release-interface.html

// http://www.linuxquestions.org/questions/programming-9/driver-development-usbhid-is-blocking-custom-device-driver-850287/

// http://stackoverflow.com/questions/3389192/register-bind-match-a-device-with-a-driver

// http://stackoverflow.com/questions/181892/problem-reading-from-a-device-with-libusb

struct usb_device * find_usb_dev(u_int16_t vendor, u_int16_t product)
{
/*	if (!lib_ready)
	{
		printf("usb library not initalized.");
		return NULL;
	}

	usb_find_busses(); usb_find_devices();

	struct usb_bus *usb_busses, *usb_bus;
	struct usb_device *usb_dev, *return_dev;

	usb_busses = usb_get_busses();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
	{
		printf("scanning usb bus %d\n", usb_bus->location);

		for (usb_dev = usb_bus->devices; usb_dev; usb_dev = usb_dev->next)
		{
			if ((usb_dev->descriptor.idVendor == vendor &&
					usb_dev->descriptor.idProduct == product))
			{
				printf("Found usb device %d\n", usb_dev->devnum);
				return_dev = usb_dev;
			}
		}
	}

	return return_dev;
*/
  return NULL;
}

int write_to_device (char *path, char *data, int len)
{
  /*
	int fd, res, pos = 0;
	char buf[256];
	struct usb_device *usb_dev;
	struct usb_dev_handle *usb_handle;

	initialize();
	if (lib_ready == 0)
	{
		printf("Failed to initalize usb library (%d)\n", res);
		return 1;
	}

	usb_dev = find_usb_dev(DEV_VENDOR, DEV_PRODUCT);

	if (usb_dev == NULL)
	{
		printf("no usb device found\n");
		return 1;
	}

	printf("Device found (0x%04x, 0x%04x) name: %s\n", usb_dev->descriptor.idVendor, usb_dev->descriptor.idProduct, usb_dev->filename);

	//usb_handle = usb_open(usb_dev);

	if (usb_handle == NULL)
	{
		printf("unable to open usb device\n");
		return 1;
	}

	//usb_detach_kernel_driver_np(usb_handle, 2);

	memset(buf, 0x0, sizeof(buf));




/*

	// REQUEST_TYPE needs to equal 0x21
	control.bRequestType = UM_CTRL_REQUEST_TYPE;
	control.bRequest = UM_CTRL_REQUEST;
	control.wValue = UM_CTRL_VALUE;
	control.wIndex = UM_CTRL_INDEX;
	control.wLength = UM_CTRL_BUFFER_SIZE;
	control.timeout =  5000;
	control.data = &buf[0];


	while (pos < len)
    {
	   memcpy(buf, &data[pos], 5);
	   pos+=5;

	   /*res = usb_control_msg(usb_handle,
	   					UM_CTRL_REQUEST_TYPE,
	   					UM_CTRL_REQUEST,
	   					UM_CTRL_VALUE,
	   					UM_CTRL_INDEX,
	   	                buf,
	   	                5,
	   	                5000);

	   if (res < 0) {
		   printf("ioctl USBDEVFS_CONTROL returned: %d\n", res);
		   break;
	   } else {
		   printf("write() wrote %d bytes\n", res);
	   }

    }



	//usb_close(usb_handle);

*/
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

		case json_type_array:
		  printf("type: json_type_array, \n");
		  break;
		}
	 }

	return 1;
}


enum ultimarc_type determine_device (json_object* jobj)
{
  enum ultimarc_type type = ultimarc_none;
  if (isIPAC(jobj))
  {
    type = ultimarc_ipac;
  }
  else if (isULTISTIK(jobj))
  {
    type = ultimarc_ultistik;
  }

  return type;
}

bool updateBoard (const char* file)
{
  bool ret = false;
  json_object *jobj = NULL;

  jobj = json_object_from_file (file);

  switch (determine_device(jobj))
  {
    case ultimarc_ipac:
      printf ("Updating IPAC board...");
      ret = updateBoardIPAC(jobj);
      break;

    case ultimarc_ultistik:
      printf ("Updating Ultistick board...");
      ret = updateBoardULTISTIK(jobj);
      break;

    default:
      printf ("ERROR: Unknown json file.  ");
      break;
  }

  if (ret)
  {
    printf ("Update done.\n");
  }
  else
  {
    printf ("Update failed!\n");
  }

  json_object_put(jobj);
  return ret;
}
