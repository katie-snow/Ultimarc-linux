/*
 * common.c
 *
 *  Created on: Mar 14, 2014
 *      Author: katie
 */

/* C */
#include <stdio.h>


#include "common.h"

struct libusb_device_handle*
openUSB(libusb_context *ctx, uint16_t vendor, uint16_t product, int interface, int autoconnect)
{
  int ret = 0;

  struct libusb_device_handle *handle = NULL;

  /* Open USB communication */
  ret = libusb_init(&ctx);
  if (ret < 0)
  {
    printf("libusb_init failed: %i\n", ret);
    goto error;
  }
  libusb_set_debug(ctx, 3);

  handle = libusb_open_device_with_vid_pid(ctx, vendor, product);
  if (!handle)
  {
    printf("Unable to open device\n");
    goto error;
  }

  if (autoconnect == 0)
  {
  /* detach the kernel driver */
  if(libusb_kernel_driver_active(handle, interface) == 1)
  {
    printf ("Kernel Driver Active\n");
    if(libusb_detach_kernel_driver(handle, interface) == 0) //detach it
      printf ("Kernel Driver Detached!\n");
  }
  }
  else
  {
    libusb_set_auto_detach_kernel_driver(handle, 1);
  }

  ret = libusb_claim_interface(handle, interface);
  if (ret < 0)
  {
    printf ("Unable to claim interface\n");
    goto error;
  }

  exit:
  return handle;

  error:
  closeUSB(ctx, handle, interface);
  return NULL;
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
