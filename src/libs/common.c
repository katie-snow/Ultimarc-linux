/*
 ============================================================================
 Name        : common.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Common functions between device libraries
 ============================================================================
 */

/* C */
#include <stdio.h>
#include <json.h>

#include "common.h"
#include "dbg.h"

#ifdef __linux__
  #define IS_LINUX 1
#else
  #define IS_LINUX 0
#endif


struct libusb_device_handle*
openUSB(libusb_context *ctx, uint16_t vendor, uint16_t product, int interface, int autoconnect)
{
  int ret = 0;

  struct libusb_device_handle *handle = NULL;

  /* Open USB communication */
  ret = libusb_init(&ctx);
  if (ret < 0)
  {
    log_err("libusb_init failed: %i.", ret);
    goto error;
  }
  libusb_set_debug(ctx, 3);

  handle = libusb_open_device_with_vid_pid(ctx, vendor, product);
  if (!handle)
  {
    log_err("Unable to open device.");
    goto error;
  }

  if (interface != -1)
  {
    if (!claimInterface(handle, interface, autoconnect))
    {
      goto error;
    }
  }

  exit:
  return handle;

  error:
  closeUSB(ctx, handle, interface);
  return NULL;
}

struct libusb_device_handle*
openUSBWithReleaseNumber(libusb_context *ctx, uint16_t vendor, uint16_t product,
                         uint16_t releaseNumber, int interface, int autoconnect)
{
  int ret = 0;

  struct libusb_device_handle *handle = NULL;

  /* Open USB communication */
  ret = libusb_init(&ctx);
  if (ret < 0)
  {
    log_err("libusb_init failed: %i.", ret);
    goto error;
  }
  libusb_set_debug(ctx, 3);

  /* Get the list of devices */

  libusb_device **list;
  ssize_t cnt = libusb_get_device_list(NULL, &list);
  ssize_t i = 0;
  int err = 0;
  if (cnt < 0) 
  {
    log_err("Unable to load device list");
    goto error;
  }

  /* Loop through all the USB devices */

  for (i = 0; i < cnt; i++) 
  {
    libusb_device *device = list[i];

    /* Get the device descriptor to get the vendor, product, and device release number */

    struct libusb_device_descriptor deviceDescriptor;

    int r = libusb_get_device_descriptor(device, &deviceDescriptor);
    if (r < 0)
    {
      continue;
    }

    if (deviceDescriptor.idVendor == vendor && 
        deviceDescriptor.idProduct == product &&
	deviceDescriptor.bcdDevice == releaseNumber)
    {
      struct libusb_device_handle *deviceHandle = NULL;

      err = libusb_open(device, &deviceHandle);
      if(err == 0)
      {
        handle = deviceHandle;
        break;
      }
    }
  }
 
  /* free the device list */

  libusb_free_device_list(list, 1);

  /* Verify we found a device */

  if (handle == NULL) 
  {
    log_err("Unable to open device.");
    goto error;
  }

  if (interface != -1)
  {
    if (!claimInterface(handle, interface, autoconnect))
    {
      goto error;
    }
  }

  exit:
  return handle;

  error:
  closeUSB(ctx, handle, interface);
  return NULL;
}

bool
claimInterface(struct libusb_device_handle *handle, int interface, bool autoconnect)
{
  int ret = 0;
  bool success = true;

  if (autoconnect == 0)
  {
    /* detach the kernel driver */
    if(libusb_kernel_driver_active(handle, interface) == 1)
    {
      log_info ("Kernel Driver Active.");
      if(libusb_detach_kernel_driver(handle, interface) == 0) //detach it
        log_info ("Kernel Driver Detached.");
    }
  }
  else
  {
    libusb_set_auto_detach_kernel_driver(handle, 1);
  }

  if (IS_LINUX)
  {
    ret = libusb_claim_interface(handle, interface);
    if (ret < 0)
    {
      log_err ("Unable to claim interface.");
      success = false;
    }
  }

  return success;
}

void
closeUSB(libusb_context *ctx, struct libusb_device_handle *handle, int interface)
{
  if (handle)
  {
    libusb_release_interface(handle, interface);
    libusb_close(handle);
  }

  if (ctx)
    libusb_exit(ctx);

}

bool checkBoardID(json_object* jobj, const char* entry)
{
  bool valid = false;
  int id = -1;

  json_object* tmp = NULL;

  if (json_object_object_get_ex(jobj, entry, &tmp))
  {
    if (json_object_get_type(tmp) == json_type_int)
    {
      id = json_object_get_int(tmp);
      if (id > 0 && id < 5)
      {
        valid = true;
      }
      else
      {
        log_err ("'%s' is not a valid entry.  Valid entries are 1 - 4", entry);
      }
    }
    else
    {
      log_err ("'%s' is not defined as an integer", entry);
    }
  }

  return valid;
}

char convertDecimalToHex(int decimal)
{
  if(decimal > 0xFF)
     decimal = 0;

  return decimal & 0xFF;
}
